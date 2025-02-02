#include "Header Files/Kernel.h"

#include "Header Files/TextPrint.h"
#include "Header Files/IDT.h"
#include "Header Files/Keyboard.h"
#include "Header Files/Memory.h"
#include "Header Files/IO.h"
#include "Header Files/Heap.h"
#include "Header Files/Typedefs.h"
#include "Header Files/Timer.h"
#include "Header Files/DiskRead.h"
#include "Header Files/FileSystem.h"

//to use drawingggg
#include "Header Files/Render.h"

#include "Header Files/Commands.h"
#include "Header Files/FileSystem.h"

//compiler
#include "Header Files/Compiler.h"

#define GET_RIP (uint32_t)({uint32_t rip; asm volatile ("lea (%%rip), %%eax\n\t" "mov %%eax, %0\n\t" : "=r"(rip)); rip;}) 

#define NULL 0



//kernel entry, called by extended_program.asm
void afficher_rigolo(uint8_t a){
    asm("nop");
    uint64_t b = 0x69;
    Print(IntToString(a), FOREGROUND_LIGHTRED);
    return;
}

void test_disk(){
    Print("\n\rlecture1...");
    uint8_t* buffer3 = (uint8_t*)calloc(512*sizeof(uint8_t));
    readDataATA(0, 1, buffer3);
    for(int i = 0; i<16; i++){
         Print(HexToString(buffer3[i]));
         Print(" ");
     }

    Print("\n\rlecture1 res: ");
    Print(HexToString(get_status()));

    Print("\n\rEcriture...");
    uint8_t* buffer_write = (uint8_t*)calloc(sizeof(uint8_t)*512);
    for(int i = 0; i<256; i++){
        buffer_write[i] = i;
    }
    for(int i = 255; i<512; i++){
        buffer_write[i] = 0x69;
    }
    buffer_write[0]= 0x69;

    writeDataATA(0, 1, buffer_write);
    Print("\n\rEcriture resultat: ");
    Print(HexToString(get_status()));


    Print("\n\rlecture2...");
    uint8_t* buffer4 = (uint8_t*)calloc(512*sizeof(uint8_t));
    readDataATA(0, 1, buffer4);
    for(int i = 0; i<16; i++){
         Print(HexToString(buffer4[i]));
         Print(" ");
     }
        Print("\n\rlecture 2 res: ");
    Print(HexToString(get_status()));
}

void setup_disk_test(){
    
    setup_root(); //attention, doit être utiliser qu'une seule fois !!!


    int t = create_folder_in_parent(0, "test");
    int bin = create_folder_in_parent(0, "bin");
    create_folder_in_parent(0, "usr");


    char* data_for_file = (char*)malloc(BLOCK_SIZE*2);
    for(int i = 0; i<BLOCK_SIZE*2; i++){
        data_for_file[i] = 'a';
    }
    data_for_file[BLOCK_SIZE*2-1] = '\0';
    write_in_data(data_for_file, "Hello world !\n\r"); //TODO this file should be the compiled version of an simple calculator
    uint16_t f = create_file_in_parent(bin, "arithm", data_for_file);
    free(data_for_file);



    create_folder_in_parent(0, "test2");

    char* s = (char*)malloc(BLOCK_SIZE*2);
    *s = 'E';
    
    read_begin_of_file(f, s, BLOCK_SIZE*2);
    Print("Read: ");
    PrintString(s);
    Print("\n\r");
    Print("\n\r");
    Print("Everything is fine\n\r");
    free(s);
    
}

void hellow(){
    Print("Helloow ^_^\n\r", FOREGROUND_LIGHTMAGENTA);
}

extern "C" void _start(){
    
    InitIDT();
    setLanguage(KBSet1::ScanCodeLookupTableAZERTY);
    MainKeyBoardHandler = KeyBoardHandler;
    InitializeHeap(0x100000, 0x100000);

    
    
    ClearScreen();
    SetCursorPosition(PosFromCoord(0, 0));
    //PrintString(Test);
    PrintString("Bienvenue sur "); PrintString("AdeOs", FOREGROUND_LIGHTGREEN); PrintString(" !\n\r\n\r");
    
    Print("Test affichage:\n\r", FOREGROUND_LIGHTCYAN);
    Print("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    Print("!\"#$%&\'()*+,-./0123456789\n\r\n\r");


    Print("Test filesystem:\n\r", FOREGROUND_LIGHTCYAN);
    //setup_disk_test();
    PrintString("Reading folder 0 (root dir)\n\r");
    uint16_t* inodes = read_folder(0);
    if(inodes == NULL){
        Print("Error reading folder\n");
        return;
    }

    for(int i = 0; false && i<MAX_INODE_PER_DIR; i++){
        Print("Value of inode ");
        Print(IntToString(i));
        Print(": ");
        Print(IntToString(i));
        Print(" ");
    }
    Print("\n\r");
    free(inodes);

    Print("Test lecture information:\n\r", FOREGROUND_LIGHTCYAN);
    file* f = read_inode_info(0);
    Print("Name of inode 0: ");
    Print(f->name);
    Print("\n\r");
    free(f);


    Print("\n\r Everything is fine\n\r", FOREGROUND_GREEN);

    //padding to see in hexfile, 64 'a'

    //get address of the function hellow
    uint64_t hellow_addr = (uint64_t)hellow;
    Print("Address of hellow: ");
    Print(HexToString(hellow_addr));
    Print("\n\r");
    hellow();
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");


    // //écrit une fonction en asm qui renvoie la valeur 0x69
    // uint8_t asm_code[] = {0xB8, 0x69, 0x00, 0x00, 0x00, 0xC3, 0xF4};
    // Print("Code de la fonction en asm: ");
    // for(int i = 0; i<6; i++){
    //     Print(HexToString(asm_code[i]));
    //     Print(" ");
    // }

    // //affiche la valeur renvoyée par la fonction
    // Print("Appel de la fonction en asm\n\r");
    // uint8_t ret = 0;
    // int n = 0x1269;
    // asm volatile ("call %1\n\t"
    //               "mov %0, %%al\n\t"
    //               : "=r"(ret)
    //               : "r"(asm_code)
    //               : "rax", "rbx", "rcx", "rdx");
    // Print("Valeur renvoyee: 0x");
    // Print(HexToString(ret));
    // Print("\n\r");


    //     asm("call %0" : : "r"(hellow_addr));

    Print("Everything is fine\n\r", FOREGROUND_LIGHTGREEN);

    //83 c0 01  = add eax, 1
    uint8_t asm_code2[] = {0x83, 0xC0, 0x01, 0xC3};
    Print("Code de la fonction en asm: ");
    for(int i = 0; i<4; i++){
        Print(HexToString(asm_code2[i]));
        Print(" ");
    }

    Print("Address of the function in asm: ");
    Print(HexToString((uint64_t)asm_code2));
    Print("\n\r");

    //affiche la valeur renvoyée par la fonction
    Print("Appel de la fonction en asm\n\r");
    char* str= "omg";
    uint64_t addstr = (uint64_t)str;
    Print("Address of the string: ");
    Print(HexToString(addstr));
    Print("\n\r");


    //pour l'appel, l'argument est 0x69 et doit être stocké dans %edi avant un call. puis on doit appeler hellow
    uint32_t ret2 = 0x12;
    asm volatile ("mov $0x69, %%eax\n\t"
                  "call *%1\n\t"
                  "mov %%eax, %0\n\t"
                  "push %%rax\n\t"
                  "push %%rbx\n\t"
                  "mov $1, %%rax\n\t"
                  "mov %2, %%rbx\n\t"
                  "int $0x80\n\t"
                  "pop %%rbx\n\t"
                  "pop %%rax\n\t"
                  "call *%3\n\t"
                  : "=b"(ret2)
                  : "c"(asm_code2) ,"m"(addstr), "m"(hellow_addr)
                  : );
    Print("Valeur renvoyee: 0x");
    Print(HexToString(ret2));
    Print("\n\r");

    


    
    //call the function hellow in assembly



    while (true)
    {
        /* code */
    }
    

    return;
    
}





/*
(FAUX ce qui suit est un mensonge):séprarer la lecture du extended program et du code C dans le bios est inutile
donc autant tout lire d'un coup (VRAI?)
actuellement le fichier fait <0x7700 d'après hxD donc on peut tout lire d'un coup
maintenat il ne reste plus qu'a comprendre pourquoi un appel de fonction fait tout crash


ERRATUM:
lire d'un coup fonctionne pour les appels exterieur.
donc on a surement une erreur quand on lit en 2 fois
il faut comprendre cette erreur pour aficher le message de la fonction test :)

cf. disk_read.asm pour les explications.
*/


/*
Idée du pseudo linker:
on rajoute les libs dans le fichier à compiler
Le compilateur s'occupe de charger l'addresse des fonctions dans les libs importante

*/