/*
    Console_.h  

    Declarations for the Console module.

    PLEASE NOTE! The added underscore in the filename is a workaround.
                 If this file is named Console.h, the program will compile 
                 and run just fine -- except that it will inexplicably
                 use 2438 extra bytes of ROM and 84 extra bytes of RAM.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

#include "Model.h"

/*
   The following 2 macros -- CONSTR(x) and ALLOC_CONSTR(p, x) -- allow for the 
   semi-transparent placement of string contants in ROM (circumventing Arduino's 
   run-time behavior of copying constants into RAM).

   Whenever a string constant is expected as an argument to a routine, the string
   literal should be encapsulated by the CONSTR() macro. For example, given the 
   2 routine prototypes:

      void iExpectAVariableString( char * )
      void iExpectAConstantString( const char * )

   and the string 
   
      "here you go"

   they should be called as follows:

      iExpectAVariableString( "here you go" );
      iExpectAConstantString( CONSTR( "here you go" ) );

   iExpectAConstantString() must read the string from ROM. This may not be an 
   option if it is a pre-compiled routine in a 3rd-party library, in which
   case use a standard Arduino char constant (which ends up in RAM).

   The CONSTR(x) macro cannot be used outside of a function, or within a list. 
   For this, use the ALLOC_CONSTR(p, x) to define a ptr to the constant string x,
   then refer to the string via its ptr in the list.

   For example given the struct:

      typedef struct { const char *a; const char *b; } twoConstantStrs;

   use

      ALLOC_CONSTR( str1, "1st string" );
      ALLOC_CONSTR( str2, "2nd string" );
      twoConstantStrs myInstance = { str1, str2 };
*/

#ifdef CONSOLE_OUTPUT

   #define CONSTR(x) PSTR(x)
   #define ALLOC_CONSTR(p, x) const char p [] PROGMEM = x

   #define about_program(x, y)                        \
                                                      \
      extern const char __PROGNAME__[];               \
      extern const char __PROGVER__[];                \
      const char __PROGNAME__[] PROGMEM = #x;         \
      const char __PROGVER__[]  PROGMEM = #y;         

#else

   #define CONSTR(x) PSTR("")
   #define ALLOC_CONSTR(p, x) const char p [] PROGMEM = ""

   #define about_program(x, y)

#endif

#ifdef USE_SERIAL_PORT

   #define set_baud_rate(x)                           \
                                                      \
      extern const unsigned long __BAUDRATE__;        \
      const unsigned long __BAUDRATE__ = x;           \

#else

   #define set_baud_rate(x)

#endif

#include "Arduino.h"
#include "Mode.h"

class Console
{
   static const int MAX_MODE = 8;      // max nesting level of console modes

   Mode   *_modeStk[ MAX_MODE+1 ];     // obj ptrs of nested console modes
   signed char modeSP;                 // stackptr for _modeStk[]

   const char *atMacro;                // ptr into macro string being executed

   boolean  output;                    // if true, console output is enabled
   boolean  oneShot;                   // if true, interpret next key-down event 
                                       // as a menu selection
   boolean  menuKeyDn;                 // if true, last key down was a menu
                                       // selection

   public:

   static const int SEAM     = 10;     // width of console seam
   char    delim;                      // character used as prompt delimiter

   Console()
   {
      delim = '>';
   }

   void  (*idle)() = NULL;             // ptr to routine to run while waiting 
                                       // for line input to complete

   void    disable();                  // disable console I/O
   void    done();
   void    enable();                   // enable console I/O
   void    exe( const char * );        // execute a macro str located in ROM
   boolean getByte( const char*, byte* );       // prompt for and get a byte (0:255)
   char    getDigit( const char*, byte );       // get '0' thru '9', (0 = aborted)
   boolean getDouble( const char*, double* );   // prompt for and get a double
   boolean getInt( const char*, int* );         // prompt for and get an int
   boolean getSByte( const char*, char* );      // prompt for and get byte (-128:127)
   boolean getStr( const char* );               // prompt for and get a String
   boolean getULong( const char*, unsigned long* ); // info on unsigned long
   void    infoByte( const char*, byte );       // print info on a named byte
   void    infoInt( const char*, int );         // print info on a named int
   void    infoDouble( const char*, double );   // print info on a named double 
   void    infoStr( const char*, const char* ); // print info on a named str
   void    infoULong( const char*, unsigned long ); // print info on named unsigned long
   void    init( Mode *mode, void (*)() );
   void    input();
   void    newline();                  // go to beginning of next line
   void    newlntab();                 // go to beginning of next line and rtab
   void    newprompt();                // display a new prompt for current mode
   void    oneShotMenu();              // interpret next key as a menu selection
   boolean ongoing( Mode * );
   void    popMode();                  // pop the top mode on the mode stack
   void    postBut( byte, butAction ); // post a button event  
   void    postKeyDn( byte, byte );    // post a key-down event  
   void    postKeyUp( byte, byte );    // post a key-up event  
   void    postPot( byte, byte );      // post a pot event  
   void    print( char );              // print a character
   void    print( char * );            // print a string
   void    print( int );               // print an int
   void    pushMode( Mode * );         // push a mode onto the mode stack
   void    romprint( const char * );   // print a string located in ROM
   byte    romstrlen( const char * );  // returns length of str in ROM
   void    rtab();                     // move to right of seam
   void    runMode( Mode * );
   void    setIdle( void (*)() );      // specify routine to run when idling
   void    setMode( Mode * );
   void    space();                    // print a space
   void    space( byte );              // print multiple spaces

   private:

   void    begInfo( const char* );     // print beginning of info cell 
   void    endInfo();                  // print end of info cell 

} ;

/* character codes for selected nonalphanumeric characters */

#define chrCR           13             // <CR>
#define chrESC          27             // <ESC>
#define chrTrigger      -2             // command: trigger 
#define chrRelease      -3             // command: release 
#define chrInfo         -4             // command: display object info
#define chrInLnfo       -5             // command: display object info inline
#define chrBrief        -6             // command: display an object brief

   // following 3 commands are issued by console on change of focus

#define focusPUSH       -7             // mode is being pushed on stack
#define focusPOP        -8             // mode is being popped from stack
#define focusRESTORE    -9             // mode is restored after pop of child


/* ------------------------      public vars      -------------------------- */

extern Console console;                // Console object

/* ----------------------     public functions     ------------------------- */

#define isDigit(x) ( x >= '0' && x <= '9' )

extern void  console_setup( Mode * );
extern void  exit_sketch();
extern void  toStr( double val, char *str );
extern char *toStr( double val );

#endif   // ifndef CONSOLE_H_INCLUDED