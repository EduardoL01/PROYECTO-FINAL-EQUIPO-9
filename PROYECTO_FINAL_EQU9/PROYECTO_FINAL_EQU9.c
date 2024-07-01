#define FCY 5000000UL
#include <p30f4011.h>
#include <libpic30.h>

_FOSC(CSW_FSCM_OFF &HS);
_FWDT(WDT_OFF);
_FBORPOR(MCLR_EN &PBOR_OFF &PWRT_64);
_FGS(CODE_PROT_OFF);

// Definiciones
#define foco1 PORTBbits.RB0
#define leds PORTBbits.RB2
#define contacto PORTBbits.RB3
#define sw1 PORTEbits.RE0
#define sw2 PORTEbits.RE1
#define sw3 PORTEbits.RE3

// PROTOTIPO DE FUNCIONES
void puertosIO();
void transmisor();
void receptor();
int xd = 0;
int dx = 0;
int px = 0;
int con = 0;

// FUNCION PRINCIPAL
void main()
{
    // EJECUCION DE FUNCIONES
    puertosIO();
    transmisor();
    receptor();

    foco1 = 1;
    leds = 0;
    contacto = 1;

    // BUCLE REPETITIVO
    while (1)
    {
        // Verifica si hay datos nuevos
        // Procesa los datos según su valor
        switch (xd)
        {
        case 100:
            switch (px)
            {
            case 40:
                foco1 = 0;
                dx = 10;
                break;
            case 45:
                foco1 = 1;
                dx = 15;
                break;
            case 50:
                leds = 0;
                dx = 20;
                break;
            case 55:
                leds = 1;
                dx = 25;
                break;
            case 60:
                contacto = 0;
                dx = 30;
                break;
            case 65:
                contacto = 1;
                dx = 35;
                break;
            }
            break;

        case 200:
            if (sw1 == 1)
            {
                foco1 = 0;
                dx = 10;
            }
            else if (sw1 == 0)
            {
                foco1 = 1;
                dx = 15;
            }
            else if (sw2 == 1)
            {
                leds = 0;
                dx = 20;
            }
            else if (sw2 == 0)
            {
                leds = 1;
                dx = 25;
            }
            else if (sw3 == 1)
            {
                contacto = 1;
                dx = 30;
            }
            else if (sw3 == 0)
            {
                contacto = 0;
                dx = 35;
            }
            break;
        }
    }
}

void puertosIO()
{
    // relay´s
    TRISBbits.TRISB0 = 0;
    ADPCFGbits.PCFG0 = 1;
    TRISBbits.TRISB2 = 0;
    ADPCFGbits.PCFG2 = 1;
    TRISBbits.TRISB3 = 0;
    ADPCFGbits.PCFG3 = 1;

    // sw
    TRISEbits.TRISE0 = 1;
    TRISEbits.TRISE1 = 1;
    TRISEbits.TRISE3 = 1;
}

// FUNCION DE CONFIGURACION DE TRANSMISOR UART
void transmisor()
{
    // Configuración del transmisor UART
    U2MODE = 0;
    U2STA = 0;
    U2BRG = 32;
    U2MODEbits.PDSEL = 0;
    U2MODEbits.STSEL = 0;
    IEC1bits.U2TXIE = 1;
    IPC6bits.U2TXIP = 4;
    U2STAbits.UTXISEL = 0;
    IFS1bits.U2TXIF = 0;
    U2MODEbits.UARTEN = 1;
    U2STAbits.UTXEN = 1;
}

// FUNCION DE CONFIGURACION DE RECEPTOR UART
void receptor()
{
    // Configuración del receptor UART
    U2MODE = 0;
    U2MODEbits.PDSEL = 0;
    U2BRG = 32;
    U2STA = 0;
    U2STAbits.URXISEL = 0;
    U2MODEbits.STSEL = 0;
    IEC1bits.U2RXIE = 1;
    IPC6bits.U2RXIP = 4;
    IFS1bits.U2RXIF = 0;
    U2MODEbits.UARTEN = 1;
    U2STAbits.UTXEN = 0;
}

// ISR - RUTINAS DE SERVICIO A LA INTERRUPCION
// ISR TRANSMISOR
void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void)
{
    U2TXREG = dx;
    IFS1bits.U2TXIF = 0;
}

// ISR RECEPTOR
void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void)
{
    xd = U2RXREG;
    px = U2RXREG; // px es la variable que se utiliza para procesar el segundo valor
    con = U2RXREG;
    IFS1bits.U2RXIF = 0;
}