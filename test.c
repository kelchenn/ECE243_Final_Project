
void disable_A9_interrupts(void);
void set_A9_IRQ_stack(void);
void config_GIC(void);
void config_KEYs(void);
void enable_A9_interrupts(void);
/* ********************************************************************************
 * This program demonstrates use of interrupts with C code. The program
 *responds
 * to interrupts from the pushbutton KEY port in the FPGA.
 *
 * The interrupt service routine for the KEYs indicates which KEY has been
 *pressed
 * on the LED display.
 ********************************************************************************/
int time = 0;
int main(void)
{
    disable_A9_interrupts(); // disable interrupts in the A9 processor
    set_A9_IRQ_stack();      // initialize the stack pointer for IRQ mode
    config_GIC();            // configure the general interrupt controller
    config_KEYs();           // configure pushbutton KEYs to generate interrupts
    enable_A9_interrupts();  // enable interrupts in the A9 processor

    int count = 0xbebc200;

    /* timer base address */
    volatile int *timer_ptr = (int *)0xFFFEC600;
    volatile int *control_ptr = (int *)0xFFFEC608;
    
    *timer_ptr = count;

    *control_ptr = *control_ptr | 0x7;

    while (1)                // wait for an interrupt
        ;
}
/* setup the KEY interrupts in the FPGA */
void config_KEYs()
{
    volatile int *KEY_ptr = (int *)0xFF200050; // pushbutton KEY base address
    *(KEY_ptr + 2) = 0xF;                      // enable interrupts for the two KEYs
}
/* This file:
 * 1. defines exception vectors for the A9 processor
 * 2. provides code that sets the IRQ mode stack, and that dis/enables
 * interrupts
 * 3. provides code that initializes the generic interrupt controller
 */
void timer_ISR(void);
void config_interrupt(int, int);
// Define the IRQ exception handler
void __attribute__((interrupt)) __cs3_isr_irq(void)
{
    // Read the ICCIAR from the CPU Interface in the GIC
    int interrupt_ID = *((int *)0xFFFEC10C);
    if (interrupt_ID == 29) // check if interrupt is from the KEYs
        timer_ISR();
    else
        while (1)
            ; // if unexpected, then stay here
    // Write to the End of Interrupt Register (ICCEOIR)
    *((int *)0xFFFEC110) = interrupt_ID;
}
// Define the remaining exception handlers
void __attribute__((interrupt)) __cs3_reset(void)
{
    while (1)
        ;
}
void __attribute__((interrupt)) __cs3_isr_undef(void)
{
    while (1)
        ;
}
void __attribute__((interrupt)) __cs3_isr_swi(void)
{
    while (1)
        ;
}
void __attribute__((interrupt)) __cs3_isr_pabort(void)
{
    while (1)
        ;
}
void __attribute__((interrupt)) __cs3_isr_dabort(void)
{
    while (1)
        ;
}
void __attribute__((interrupt)) __cs3_isr_fiq(void)
{
    while (1)
        ;
}
/*
 * Turn off interrupts in the ARM processor
 */
void disable_A9_interrupts(void)
{
    int status = 0b11010011;
    asm("msr cpsr, %[ps]"
        :
        : [ps] "r"(status));
}
/*
 * Initialize the banked stack pointer register for IRQ mode
 */
void set_A9_IRQ_stack(void)
{
    int stack, mode;
    stack = 0xFFFFFFFF - 7; // top of A9 onchip memory, aligned to 8 bytes
    /* change processor to IRQ mode with interrupts disabled */
    mode = 0b11010010;
    asm("msr cpsr, %[ps]"
        :
        : [ps] "r"(mode));
    /* set banked stack pointer */
    asm("mov sp, %[ps]"
        :
        : [ps] "r"(stack));
    /* go back to SVC mode before executing subroutine return! */
    mode = 0b11010011;
    asm("msr cpsr, %[ps]"
        :
        : [ps] "r"(mode));
}
/*
 * Turn on interrupts in the ARM processor
 */
void enable_A9_interrupts(void)
{
    int status = 0b01010011;
    asm("msr cpsr, %[ps]"
        :
        : [ps] "r"(status));
}
/*
 * Configure the Generic Interrupt Controller (GIC)
 */
void config_GIC(void)
{
    config_interrupt(29, 1); // configure the FPGA KEYs interrupt (73)
    // Set Interrupt Priority Mask Register (ICCPMR). Enable interrupts of all
    // priorities
    *((int *)0xFFFEC104) = 0xFFFF;
    // Set CPU Interface Control Register (ICCICR). Enable signaling of
    // interrupts
    *((int *)0xFFFEC100) = 1;
    // Configure the Distributor Control Register (ICDDCR) to send pending
    // interrupts to CPUs
    *((int *)0xFFFED000) = 1;
}
/*
 * Configure Set Enable Registers (ICDISERn) and Interrupt Processor Target
 * Registers (ICDIPTRn). The default (reset) values are used for other registers
 * in the GIC.
 */
void config_interrupt(int N, int CPU_target)
{
    int reg_offset, index, value, address;
    /* Configure the Interrupt Set-Enable Registers (ICDISERn).
     * reg_offset = (integer_div(N / 32) * 4
     * value = 1 << (N mod 32) */
    reg_offset = (N >> 3) & 0xFFFFFFFC;
    index = N & 0x1F;
    value = 0x1 << index;
    address = 0xFFFED100 + reg_offset;
    /* Now that we know the register address and value, set the appropriate bit */
    *(int *)address |= value;
    /* Configure the Interrupt Processor Targets Register (ICDIPTRn)
     * reg_offset = integer_div(N / 4) * 4
     * index = N mod 4 */
    reg_offset = (N & 0xFFFFFFFC);
    index = N & 0x3;
    address = 0xFFFED800 + reg_offset + index;
    /* Now that we know the register address and value, write to (only) the
     * appropriate byte */
    *(char *)address = (char)CPU_target;
}
/********************************************************************
 * Timer - Interrupt Service Routine
 *******************************************************************/

const short int bit_codes[10] = {0x3f, 0x6, 0x5b, 0x4f,
   							0x66, 0x6d, 0x7d, 0x7, 0x7f, 0x67};

int timer_nums[4] = {0, 0, 0, 0};

short int timer_hex[4] = {0x0, 0x0, 0x0, 0x0};

void timer_ISR(void)
{

    time++;

    /* timer base address */
    volatile int *timer_ptr = (int *)0xFFFEC600;
    volatile int *f_ptr = (int *)0xFFFEC60C;
	
    volatile int *LEDR_ptr = (int *)0xFF200000;
	
    int f = *f_ptr;
	*f_ptr = f;

    *LEDR_ptr = time;

    /* HEX display base address */
    volatile int *HEX3_HEX0_ptr = (int *)0xFF200020;
    volatile int *HEX5_HEX4_ptr = (int *)0xFF200030;

    int remainder;
    int digit_count = 0;
	
	int num = time;

    while (num != 0) {
      
      // get remainder
      remainder = num % 10;
    
      timer_nums[digit_count] = remainder;

      num = num / 10;
      digit_count++;
    }

    for (int j = digit_count; j > -1; j--) {
   	 timer_hex[j] = bit_codes[timer_nums[j]];
    }

    int pattern = 0;

    for (int j = 4; j >-1; j--) {
   		pattern = pattern | (timer_hex[j]);
   	 
   		if (j > 0) {
   			pattern = pattern << 8;
   		}
   	}
    
   	*HEX3_HEX0_ptr = pattern;

    return;
}
