// This is where you put your VM code.
// I am trying to follow the coding style of the original.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "minivm.h"

#define NUM_REGS   (256)
#define NUM_FUNCS  (256)

// Global variable that indicates if the process is running.
static bool is_running = true;
uint8_t *mem;
uint32_t * pc;
uint32_t *code_buf;
int file_len;

void usageExit() {
    printf("./interpreter [.mini file]\n");
    exit(1);
}
uint8_t mem_read(uint32_t addr){
    if(addr > 8192)
    {
        puts("Memory is out of bound!");
        exit(1);
    }
    return mem[addr];
}
void mem_write(uint32_t addr, uint8_t value){
    if(addr > 8192)
    {
        puts("Memory is out of bound!");
        exit(1);
    }
    mem[addr] = value;
    return;    
}

void halt(struct VMContext* ctx, const uint32_t instr) {
    is_running = false;
}

void load(struct VMContext* ctx, const uint32_t instr) {
    int src = EXTRACT_B2(instr);
    int dst = EXTRACT_B1(instr);
    uint32_t addr = ctx->r[src].value;
    ctx->r[dst].value = (uint32_t)mem_read(addr);
}

void store(struct VMContext* ctx, const uint32_t instr) {
    int src = EXTRACT_B2(instr);
    int dst = EXTRACT_B1(instr);
    uint32_t addr = ctx->r[dst].value;
    mem_write(addr, EXTRACT_B0(ctx->r[src].value));
}

void move(struct VMContext* ctx, const uint32_t instr) {
    int src = EXTRACT_B2(instr);     
    int dst = EXTRACT_B1(instr);
    ctx->r[dst].value = ctx->r[src].value;
}

void puti(struct VMContext* ctx, const uint32_t instr) {
    int dst = EXTRACT_B1(instr);
    ctx->r[dst].value = EXTRACT_B2(instr);
}

void add(struct VMContext* ctx, const uint32_t instr) {
    int src1 = EXTRACT_B2(instr);
    int src2 = EXTRACT_B3(instr);
    int dst = EXTRACT_B1(instr);
    ctx->r[dst].value = (ctx->r[src1].value) + (ctx->r[src2].value);
}

void sub(struct VMContext* ctx, const uint32_t instr) {
    int src1 = EXTRACT_B2(instr);
    int src2 = EXTRACT_B3(instr);
    int dst = EXTRACT_B1(instr);
    ctx->r[dst].value = (ctx->r[src1].value) - (ctx->r[stc2].value);
}

void gt(struct VMContext* ctx, const uint32_t instr) {
    int target1 = EXTRACT_B2(instr);
    int target2 = EXTRACT_B3(instr);
    int dst = EXTRACT_B1(instr);
    if(ctx->r[target1].value > ctx->r[target2].value)
        ctx->r[dst].value = 1;
    else
        ctx->r[dst].value = 0;
}

void ge(struct VMContext* ctx, const uint32_t instr) {
    int target1 = EXTRACT_B2(instr);
    int target2 = EXTRACT_B3(instr);
    int dst = EXTRACT_B1(instr);    
    if(ctx->r[target1].value >= ctx->r[target2].value)
        ctx->r[dst].value = 1;
    else
        ctx->r[dst].value = 0;
}

void eq(struct VMContext* ctx, const uint32_t instr) {
    int target1 = EXTRACT_B2(instr);
    int target2 = EXTRACT_B3(instr);
    int dst = EXTRACT_B1(instr);
    if(ctx->r[target1].value == ctx->r[target2].value)
        ctx->r[dst].value = 1;
    else
        ctx->r[dst].value = 0;
}

void ite(struct VMContext* ctx, const uint32_t instr) {
    int target = EXTRACT_B1(instr);

    if(EXTRACT_B2(instr) >= (file_len/4) || EXTRACT_B3(instr) >= (file_len/4)) {
        puts("operand is out of code bound");
        exit(1);
    }

    if(ctx->r[target].value > 0)
        pc = &code_buf[EXTRACT_B2(instr)-1];
    else if(ctx->r[target].value == 0)
        pc = &code_buf[EXTRACT_B3(instr)-1];
}

void jump(struct VMContext* ctx, const uint32_t instr) {
    if(EXTRACT_B1(instr) >= (file_len/4)) {
        puts("operand is out of code bound");
        exit(1);
    }
    pc = &code_buf[EXTRACT_B1(instr)-1];   
}

void puts(struct VMContext* ctx, const uint32_t instr) {
    int target = EXTRACT_B1(instr);
    uint32_t addr = ctx->r[target].value;
    
    if(addr > 8192)
    {
        puts("Memory is out of bound!");
        exit(1);
    }

    printf("%s\n", (char*)(mem+addr));
}

void gets(struct VMContext* ctx, const uint32_t instr) {
    int target = EXTRACT_B1(instr);
    uint32_t addr = ctx->r[target].value;
    
    if(addr > 8192)
    {
        puts("Memory is out of bound!");
        exit(1);
    }

    scanf("%s", (char*)(mem+addr));
}

void initFuncs(FunPtr *f, uint32_t cnt) {
    uint32_t i;
    for (i = 0; i < cnt; i++) {
        f[i] = NULL;
    }
    
    f[0x00] = halt;
    f[0x10] = load;
    f[0x20] = store;
    f[0x30] = move;
    f[0x40] = puti;
    f[0x50] = add;
    f[0x60] = sub;
    f[0x70] = gt;
    f[0x80] = ge;
    f[0x90] = eq;
    f[0xa0] = ite;
    f[0xb0] = jump;
    f[0xc0] = puts;
    f[0xd0] = gets;
}

void initRegs(Reg *r, uint32_t cnt)
{
    uint32_t i;
    for (i = 0; i < cnt; i++) {
        r[i].type = 0;
        r[i].value = 0;
    }
}

int main(int argc, char** argv) {
    VMContext vm;
    Reg r[NUM_REGS];
    FunPtr f[NUM_FUNCS];
    FILE* bytecode;
    
    // There should be at least one argument.
    if (argc < 2) usageExit();

    mem = (uint8_t*)(malloc(8193));
    // Initialize registers.
    initRegs(r, NUM_REGS);
    // Initialize interpretation functions.
    initFuncs(f, NUM_FUNCS);
    // Initialize VM context.
    initVMContext(&vm, NUM_REGS, NUM_FUNCS, r, f);

    // Load bytecode file
    bytecode = fopen(argv[1], "rb");
    if (bytecode == NULL) {
        perror("fopen");
        return 1;
    }

    // file size -> malloc(filesize);
    // uint32_t *code_buf = aasdf;
    // fread(xx, code_buf, xx,, xx);

    fseek(bytecode, 0L, SEEK_END);
    file_len = ftell(bytecode);
    rewind(bytecode);

    code_buf = malloc(file_len);
    fread(code_buf, 1, file_len, bytecode);

    while (is_running) {
        stepVMContext(&vm, &pc);
    }

    fclose(bytecode);
    free(mem);
    // Zero indicates normal termination.
    return 0;
}
