#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/queue.h>

#include <ncurses.h>

typedef enum {
    LD = 1,
    ADD = 2,
    ST = 3,
    INC = 4,
    DEC = 5,
    MV = 6,
    JZ = 7,
    JMP = 8,
    JNZ = 9,
    SUB = 10,
    RAND = 11,
    SPAWN = 12,
    LDC = 13,
    MUTATE = 14
} instruction_t;

#define FUEL 512

const uint32_t memory_limit = 0xFFFFFF;
uint32_t *memory; 


typedef struct thread_t {
    uint32_t pc; // program counter
    uint32_t acc; // accumulator
    uint32_t x; // X register
    uint32_t y; // Y register
    uint32_t fuel;
    uint32_t age;
    LIST_ENTRY(thread_t) entries;
} thread_t;

LIST_HEAD(theads_t, thread_t) threads = LIST_HEAD_INITIALIZER(threads);

typedef enum {
    MEM_OK,
    MEM_ERROR
} memory_error_t;

int
mem_fetch(uint32_t addr, uint32_t *out) {
    if (addr > memory_limit)
        return MEM_ERROR;

    *out = memory[addr%(memory_limit+1)];
    return MEM_OK;
}

int
mem_store(uint32_t addr, uint32_t *in) {
    if (addr > memory_limit)
       return MEM_ERROR;

    memory[addr%(memory_limit+1)] = *in;
    return MEM_OK;
}

#define GET_DATA(pc, val) \
    do {                                        \
        if (mem_fetch((pc), (val)) != MEM_OK) { \
            kill_thread(t);                     \
            return;                             \
        }                                       \
    } while(0)

#define PUT_DATA(pc, val) \
    do {                                        \
        if (mem_store((pc), (val)) != MEM_OK) { \
            kill_thread(t);                     \
            return;                             \
        }                                       \
    } while(0)

uint32_t thread_count;
uint32_t max_threads;

uint32_t spawned = 0;
uint32_t died = 0;

void
new_thread(uint32_t addr, uint32_t fuel)
{
    thread_t *t;
    if (thread_count >= max_threads)
        return;
    spawned++;
    thread_count++;
    t = calloc(1, sizeof(*t));
    t->pc = addr;
    t->fuel = fuel;
    LIST_INSERT_HEAD(&threads, t, entries);
    return;
}

void
kill_thread(thread_t *t)
{
    died++;
    thread_count--;
    LIST_REMOVE(t, entries);
    free(t);
    return;
}


typedef enum {
    NONE = 0,
    REG_A = 1,
    REG_X = 2,
    REG_Y = 4,
    REG_PC = 8
} reg_t;

#define PAIR_REGS(X, Y) ((X) << 4 | (Y))
#define LEFT_REG(X) ((X) >> 4)
#define RIGHT_REG(X) ((X) & 0x0F)

#define INSTR(X) ((X) & 0xFF)
#define REGS(X) (((X) & 0x0000FF00) >> 8)

uint32_t *
get_register(thread_t *t, reg_t r)
{
    switch(r) {
    case REG_A:
        return &(t->acc);
    case REG_X:
        return &(t->x);
    case REG_Y:
        return &(t->y);
    case REG_PC:
        return &(t->pc);
    default:
        break;
    }

    return NULL;
}

void execute(thread_t *t) {
    uint32_t val;
    uint32_t addr;
    uint32_t *left = NULL;
    uint32_t *right = NULL;

    t->age++;

    if (t->age > 1000000) {
        kill_thread(t);
        return;
    }

    if (t->fuel == 0 || t->fuel >= 16 * FUEL) {
        kill_thread(t);
        return;
    }
   
    t->fuel--;

    GET_DATA(t->pc, &val);
//    fprintf(stderr, "thread (%p): pc: %p, acc: %d instr: %d x: %d y: %d\n", t, t->pc,
//            t->acc, val, t->x, t->y);
    left = get_register(t, LEFT_REG(REGS(val)));
    right = get_register(t, RIGHT_REG(REGS(val)));

    switch(INSTR(val)) {
    case MUTATE:
        t->fuel += 16;

        if (left == NULL) {
            kill_thread(t);
            return;
        }
        if (arc4random_uniform(128) == 3)
            *left = *left ^ arc4random_uniform(0xFFFFFFFF);
 
        if (arc4random_uniform(128) == 3)
            *left = *left ^ (1 << (arc4random_uniform(31)));
        break;

    case SPAWN:
        GET_DATA((t->pc) + 1, &val);
        if (left == NULL)
            new_thread(val, t->fuel/2);
        else
            new_thread(*left, t->fuel/2);

        t->fuel /= 4;

        break;

    case RAND:
        if (left == NULL) {
            kill_thread(t);
            return;
        }
        *left = arc4random_uniform(65536);
        break;

    case DEC:
        if (left == NULL) {
            kill_thread(t);
            return;
        }

        (*left)--;
        break;

    case INC:
        if (left == NULL) {
            kill_thread(t);
            return;
        }

        (*left)++;
        break;
    
    case MV:
        if (left == NULL || right == NULL) {
            kill_thread(t);
            return;
        }
        
        *left = *right;
        break;

    case ST:
        t->fuel += 8;
        if (left == NULL) {
            kill_thread(t);
            return;
        }

        /* XXX: I'm not really sure what's the semantics here shrug.gif */
        if (right == NULL) {
            GET_DATA((t->pc) + 1, &val);
            PUT_DATA(val, left);
        } else
            PUT_DATA(*left, right);
        
        break;

    case LDC:
        if (left == NULL) {
            kill_thread(t);
            return;
        }

        GET_DATA((t->pc) + 1, left);
        break;

    case LD:
        t->fuel += 8;
        if (left == NULL) {
            kill_thread(t);
            return;
        }

        if (right == NULL) {
            GET_DATA((t->pc) + 1, &val);
            GET_DATA(val, &val);
        } else
            GET_DATA(*right, &val);

        *left = val;
        break;

    case SUB:
        if (left == NULL) {
            kill_thread(t);
            return;
        }

        if (right == NULL)
            GET_DATA((t->pc) + 1, &val);
        else
            val = *right;

        *left -= val;
        break;

    case JMP:
        if (left != NULL)
            val = *left;
        else
            GET_DATA((t->pc) + 1, &val);

        t->pc -= val+1;
        break;

    case JZ:
        if (left == NULL) {
            kill_thread(t);
            return;
        }

        GET_DATA((t->pc) + 1, &val);

        if (*left == 0)
            t->pc -= val+1;

        break;

    case JNZ:
        if (left == NULL) {
            kill_thread(t);
            return;
        }

        GET_DATA((t->pc) + 1, &val);

        if (*left != 0)
            t->pc -= val+1;

        break;

    case ADD:
        if (left == NULL) {
            kill_thread(t);
            return;
        }

        if (right == NULL)
            GET_DATA((t->pc) + 1, &val);
        else
            val = *right;

        *left += val;
        break;
 
    default:
        kill_thread(t);
        break;
    }

    t->pc += 2;

    return;
}

void
print_memory(int level)
{
    int i, offset = 0, x, y;
    uint8_t acc;
    uint32_t max_fuel = 0;
    char printable[] = ".,-[];'>4172472389jfeneqiovhq89fvvrvWVRwkvpwe";

    thread_t *curr;
    LIST_FOREACH(curr, &threads, entries) {
        if (max_fuel < curr->fuel)
            max_fuel = curr->fuel;
    }

    clear();
    printw("\nthreads: %d level: %d\n", thread_count, level);
    printw("spawned: %d died: %d\n", spawned, died);
    printw("max fuel: %d\n\n", max_fuel);

    for (x = 0; x < 32; x++) {
        acc = 0;
        for (y = 0 ; y < 32; y++) {
            for (i = 0; i < 64; i++) {
                acc += memory[x*2048+64*y+i];
            }
            printw("%c", printable[acc % (strlen(printable))]);
        }
        printw("\n");
    }
    refresh();

    return;
}

int
main(int argc, char **argv)
{
    FILE *fh;
    thread_t *curr, *thread_tmp;
    max_threads = 65536;

    int spin = 0;
    memory = calloc(memory_limit, sizeof(*memory));
    LIST_INIT(&threads);
    new_thread(0, FUEL);

    fh = fopen(argv[1], "r");
    fread(memory, 1, 1024, fh);
    fclose(fh);

    initscr();

    while(!LIST_EMPTY(&threads)) {
        spin++;
        LIST_FOREACH_SAFE(curr, &threads, entries, thread_tmp)
            execute(curr);
        if (spin % 10000 == 0) {
            print_memory(spin);
        }
    }

    getch();
    endwin();

    return 0;
}
