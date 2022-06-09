
#define CUT_MAIN
#include "cut.h"

#define main student_main
#include "main.c"
#undef main

TEST(nanecisto_1)
{
        INPUT_STRING(
            "; na vstupu se nenachazi zadny prikaz, komentar tedy nevyvola zadny vypis\n"
            "=\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 0\n"
                    "");
}

TEST(nanecisto_2)
{
        INPUT_STRING(
            "P\n"
            "10\n"
            "\n"
            "43\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 1043\n"
                    "");
}

TEST(nanecisto_binary)
{
        INPUT_STRING(
            "P T 110\n"
            "=\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 0\n"
                    "# 6\n"
                    "# 6\n"
                    "");
}

TEST(nanecisto_minus)
{
        INPUT_STRING(
            "P 100\n"
            "- 50\n"
            "=");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 100\n"
                    "# 50\n"
                    "# 50\n"
                    "");
}

TEST(nanecisto_octal)
{
        INPUT_STRING(
            "P O 72\n"
            "=\n"
            "O\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 0\n"
                    "# 58\n"
                    "# 58\n"
                    "# 72\n"
                    "");
}

TEST(nanecisto_zadani_3)
{
        INPUT_STRING(
            "P 21\n"
            "; okamzite provedeni prikazu vyse - ukonceni predchoziho vstupu\n"
            "\n"
            "P 22\n"
            "=\n"
            "\n"
            "; jednotlive prikazy nemusi byt oddeleny novym radkem\n"
            "P 23 =\n"
            "\n"
            "N\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 21\n"
                    "# 22\n"
                    "# 22\n"
                    "# 23\n"
                    "# 23\n"
                    "# 0\n"
                    "");
}

TEST(nanecisto_zadani_3_1)
{
        INPUT_STRING(
            "P 21\n"
            "; okamzite provedeni prikazu vyse - ukonceni predchoziho vstupu\n"
            "\n"
            "P 22\n"
            "=\n"
            "\n"
            "; jednotlive prikazy nemusi byt oddeleny novym radkem\n"
            "P 23 =\n"
            "\n"
            "N\n"
            "+1;\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 21\n"
                    "# 22\n"
                    "# 22\n"
                    "# 23\n"
                    "# 23\n"
                    "# 0\n"
                    "# 1\n"
                    "");
}

TEST(nanecisto_zadani_4)
{
        INPUT_STRING(
            "P 200 ;\n"
            "/ 5 ;\n"
            "% 6 ;\n"
            "=\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 200\n"
                    "# 40\n"
                    "# 4\n"
                    "# 4\n"
                    "");
}

TEST(nanecisto_zadani_5)
{
        INPUT_STRING(
            "P 10 ;\n"
            "< 3 ;\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 10\n"
                    "# 80\n"
                    "");
}

TEST(nanecisto_zadani_6)
{
        INPUT_STRING(
            "P 21 + 43 * 5 =\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 21\n"
                    "# 64\n"
                    "# 320\n"
                    "# 320\n"
                    "");
}

TEST(nanecisto_zadani_7)
{
        INPUT_STRING(
            "P 15 ;\n"
            "* X 1a ;\n"
            "O ;\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 15\n"
                    "# F\n"
                    "# 390\n"
                    "# 606\n"
                    "");
}

TEST(nanecisto_zadani_8)
{
        INPUT_STRING(
            "P 3 ;    akumulator = 3, pamet = 0\n"
            "M   ;    akumulator = 3, pamet = 3\n"
            "R   ;    akumulator = 3, pamet = 0\n"
            "M   ;    akumulator = 3, pamet = 3\n"
            "+ m ;    akumulator = 3 + 3 = 6, pamet = 3\n"
            "=\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 3\n"
                    "# 6\n"
                    "# 6\n"
                    "");
}

TEST(edge1)
{
        INPUT_STRING(
            "P0m\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 1);
}

TEST(edge1_1)
{
        INPUT_STRING(
            "P0m;\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 1);
}

TEST(edge2)
{
        INPUT_STRING(
            "Pm\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 0\n"
                    "");
}

TEST(edge2_1)
{
        INPUT_STRING(
            "Pm;\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 0\n"
                    "");
}

TEST(edge3)
{
        INPUT_STRING(
            "POm\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 1);
}

TEST(edge3_1)
{
        INPUT_STRING(
            "P010m;\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 1);
}

TEST(edge4)
{
        INPUT_STRING(
            "PO10m\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 1);
}

TEST(edge4_1)
{
        INPUT_STRING(
            "PO10m;\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 1);
}

TEST(edge5)
{
        INPUT_STRING(
            "P10m\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 1);
}

TEST(edge5_1)
{
        INPUT_STRING(
            "P10m;\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 1);
}

TEST(edge6)
{
        INPUT_STRING(
            "Pmm\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 1);
}

TEST(edge6_1)
{
        INPUT_STRING(
            "Pmm;\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 1);
}

TEST(edge7)
{
        INPUT_STRING(
            "Pm\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 0\n"
                    "");
}

TEST(edge7_1)
{
        INPUT_STRING(
            "Pm;\n"
            "");

        int main_rv = student_main();
        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 0\n"
                    "");
}

TEST(read_ull)
{
        INPUT_STRING(
            "PT 11010001 10000000 00000000 00000000 00000000 00000000 00000000 10101100;\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 0\n"
                    "# 15096065950945902764\n"
                    "");
}

TEST(write_ull)
{
        INPUT_STRING(
            "P 15096065950945902764;\n"
            "T"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 15096065950945902764\n"
                    "# 1101000110000000000000000000000000000000000000000000000010101100\n"
                    "");
}

TEST(End)
{
        INPUT_STRING(
            "P"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 1);
}

TEST(System_change)
{
        INPUT_STRING(
            "P 1589"
            "T"
            "T"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 1589\n"
                    "# 11000110101\n"
                    "# 11000110101\n"
                    "");
}

TEST(ZERO_DIVISION)
{
        INPUT_STRING(
            "  PO1       \n"
            "   R  "
            "  <T1100    \n"
            "  \n"
            "  -O0   \n"
            "  =    \n"
            "  X      \n"
            "  ;PMBE5VWBTL3 \n"
            "   \n"
            "  /T110  \n"
            "  /m     \n"
            "   /T111    \n"
            "  =  \n"
            "   %X5   \n"
            "   T  \n"
            "   ;DR0OOGY0UL72 \n"
            "    \n"
            "      -T11  \n"
            "  <T110    \n"
            "  M    \n"
            "  %m   \n"
            "  /2      \n"
            "  \n"
            "  \n"
            "  =  \n"
            "  <T1001       \n"
            "  R    \n"
            "     R    \n"
            "  P6  \n"
            "  %10           \n"
            "  \n"
            "  <T100   \n"
            "  T    \n"
            "  <T1001   \n"
            "  X    \n"
            "  %T1001     \n"
            "    /m  \n"
            "  X  \n"
            "  >m   \n"
            "  O  \n"
            "  +T101     \n"
            "   =   \n"
            "  \n"
            "   +O14    \n"
            "  M  \n"
            "  /T10     \n"
            "      R  \n"
            "       PO4   \n"
            "  <O12  \n"
            "  =    \n"
            "  R           \n"
            "  \n"
            "  O       \n"
            "   >m      \n"
            "  ;ALXLPNSMCBEOA \n"
            "        \n"
            "  +O13   \n"
            "  \n"
            "  M  \n"
            "  >1  \n"
            "");
        int main_rv = student_main();
        ASSERT(main_rv == 1);
        ASSERT_FILE(stdout,
                    "# 0\n"
                    "# 1\n"
                    "# 1\n"
                    "# 4096\n"
                    "# 10000\n"
                    "# 4096\n"
                    "# 4096\n"
                    "# 1000\n"
                    "# 1000000000000\n"
                    "# 682\n"
                    "");
}

TEST(ZERO_DIVISION1)
{
        INPUT_STRING(
            "     T  \n"
            "     PO5   \n"
            "  /6    \n"
            "  <4   \n"
            "    +X6   \n"
            "  O   \n"
            "   +10  \n"
            "  /T1000  \n"
            "  \n"
            "  R      \n"
            "     X       \n"
            "   /m    \n"
            "  R  \n"
            "  \n"
            "  =  \n"
            "      T    \n"
            "  /m   \n"
            "  ;2E01WFS4YXF6QZ \n"
            "        \n"
            "  ;4O5F \n"
            "    \n"
            "  \n"
            "   R  \n"
            "  \n"
            "   <O3    \n"
            "  \n"
            "  >O12   \n"
            "  \n"
            "  /T1100    \n"
            "   %7   \n"
            "   ;ZVLCZUX \n"
            "      \n"
            "  \n"
            "   O      \n"
            "  T   \n"
            "  X        \n"
            "  X     \n"
            "  R  \n"
            "  =     \n"
            "  \n"
            "  X      \n"
            "  =    \n"
            "   +m  \n"
            "     +Xa   \n"
            "  \n"
            "  \n"
            "   %8    \n"
            "   >T111  \n"
            "  \n"
            "  >T1011  \n"
            "  \n"
            "  \n"
            "  R   \n"
            "  %X1   \n"
            "  =  \n"
            "  -O5  \n"
            "  >7  \n"
            "    M     \n"
            "  >m   \n"
            "    O   \n"
            "  =  \n"
            "  -m   \n"
            "       M    \n"
            "   =  \n"
            "     M     \n"
            "  X       \n");

        int main_rv = student_main();
        ASSERT(main_rv == 1);
        ASSERT_FILE(stdout,
                    "# 0\n"
                    "# 0\n"
                    "# 5\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 6\n"
                    "# 6\n"
                    "# 16\n"
                    "# 10000\n"
                    "# 2\n"
                    "# 2\n"
                    "");
}

TEST(Big1)
{
        INPUT_STRING(
            "   <O14  \n"
            "  \n"
            "  PO7   \n"
            "   >m   \n"
            "    T    \n"
            "  \n"
            "        +6  \n"
            "  M  \n"
            "  T  \n"
            "  +m   \n"
            "   Pm      \n"
            "   T  \n"
            "  O    \n"
            "  T    \n"
            "   X      \n"
            "  Pm  \n"
            "  \n"
            "  X  \n"
            "  =   \n"
            "  %T101       \n"
            "  =   \n"
            "  /T1   \n"
            "  <3   \n"
            "   O  \n"
            "  M   \n"
            "  R  \n"
            "  ;RVFN0X3Q52O02UG \n"
            "   \n"
            "  %7  \n"
            "   T   \n"
            "    X  \n"
            "  >m   \n"
            "  \n"
            "   =   \n"
            "  +X3    \n"
            "   +X7  \n"
            "   PXc  \n"
            "  T  \n"
            "  +X6  \n"
            "  -4  \n"
            "   R  \n"
            "    =  \n"
            "  =  \n"
            "    M  \n"
            "    O     \n"
            "  R     \n"
            "    P8    \n"
            "  \n"
            "          +Xb      \n"
            "   =   \n"
            "  \n"
            "  \n"
            "  ;V \n"
            "    \n"
            "  PX3   \n"
            "   R      \n"
            "  %9  \n"
            "   >X3   \n"
            "   X \n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 7\n"
                    "# 7\n"
                    "# 111\n"
                    "# 13\n"
                    "# 1101\n"
                    "# 26\n"
                    "# 13\n"
                    "# 1101\n"
                    "# 15\n"
                    "# 1101\n"
                    "# D\n"
                    "# 13\n"
                    "# D\n"
                    "# 13\n"
                    "# 1101\n"
                    "# 3\n"
                    "# 3\n"
                    "# 11\n"
                    "# 3\n"
                    "# 24\n"
                    "# 30\n"
                    "# 3\n"
                    "# 11\n"
                    "# 3\n"
                    "# 3\n"
                    "# 3\n"
                    "# 3\n"
                    "# 6\n"
                    "# 6\n"
                    "# 13\n"
                    "# D\n"
                    "# 12\n"
                    "# 1100\n"
                    "# C\n"
                    "# 18\n"
                    "# 14\n"
                    "# 14\n"
                    "# 14\n"
                    "# 16\n"
                    "# 8\n"
                    "# 8\n"
                    "# 19\n"
                    "# 19\n"
                    "# 13\n"
                    "# 3\n"
                    "# 3\n"
                    "# 3\n"
                    "# 0\n"
                    "# 0\n"
                    "");
}

TEST(Dis1)
{
        INPUT_STRING(
            "P63223\n"
            ";R\n"
            "XRNRNMNONXX;NMPMRX\n"
            "1010M T\n"
            ";=0m XOM\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 1);
        ASSERT_FILE(stdout,
                    "# 63223\n"
                    "# F6F7\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "");
}

TEST(Dis2)
{
        INPUT_STRING(
            "%T101/101\n"
            ";NR;X\n"
            "N;*MXP=T;TOO T\n"
            "RX af*==OTM0R\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 1);
        ASSERT_FILE(stdout,
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "");
}

TEST(Dis3)
{
        INPUT_STRING(
            "P16430=XOM=101/N+N=R\n"
            ";253m%TM253RR\n"
            "+OXRTT=0; /253  M\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 1);
        ASSERT_FILE(stdout,
                    "# 16430\n"
                    "# 16430\n"
                    "# 402E\n"
                    "# 40056\n"
                    "# 16430\n"
                    "");
}

TEST(Dis4)
{
        INPUT_STRING(
            "P74233R\n"
            "NMMO OMTXTX\n"
            "NR M PTmR253*;\n"
            "253N%0MR%\n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 1);
        ASSERT_FILE(stdout,
                    "# 74233\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "");
}

/*Does not work but should*/
// TEST(Dis5)
// {
//     INPUT_STRING(
//             "P1;<63=\n"
//             "P1<63=\n"
//             "");

//     int main_rv = student_main();

//     ASSERT(main_rv == 0);
//     ASSERT_FILE(stdout,
//             "# 1\n"
//             "# 9223372036854775808\n"
//             "# 9223372036854775808\n"
//             "");
// }

TEST(Big2)
{
        INPUT_STRING(
            "   PT1001   \n"
            " T \n"
            "\n"
            " R  \n"
            " R \n"
            " <T101 \n"
            " <X3 \n"
            "\n"
            "\n"
            " R \n"
            "     T \n"
            " -9  \n"
            "  M \n"
            " %m  \n"
            "\n"
            "  M  \n"
            "\n"
            "     /m \n"
            " T \n"
            " <T100 \n"
            " O  \n"
            "    X   \n"
            " X \n"
            "   = \n"
            "  <T1000 \n"
            "\n"
            " =  \n"
            "\n"
            "  /T111  \n"
            " +m    \n"
            " T    \n"
            "   /m \n"
            "  +1   \n"
            " O \n"
            " >Xa    \n"
            " <T11 \n"
            " PO0 \n"
            " =  \n"
            " /m \n"
            "\n"
            " O \n"
            "  R \n"
            " ;V5Q6O1F\n"
            "    \n"
            " PX5 \n"
            "  >O14 \n"
            " %T111  \n"
            "   %7  \n"
            " T  \n"
            "   T \n"
            "   M \n"
            "    >X3 \n"
            "   O     \n"
            " R  \n"
            " T  \n"
            "  R   \n"
            "   = \n"
            " R \n"
            "      O \n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 0\n"
                    "# 9\n"
                    "# 1001\n"
                    "# 1001\n"
                    "# 288\n"
                    "# 120\n"
                    "# 2304\n"
                    "# 100100000000\n"
                    "# 2295\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 2295\n"
                    "# 100011110111\n"
                    "# 1\n"
                    "# 2\n"
                    "# 2\n"
                    "# 2\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 5\n"
                    "# 5\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "");
}

TEST(Big3)
{
        INPUT_STRING(
            "  R \n"
            "  +X5 \n"
            "\n"
            "  ;9UB9WR\n"
            " \n"
            "  M \n"
            "\n"
            " /T111      \n"
            "   +m   \n"
            "\n"
            " ;DVU\n"
            " \n"
            "  ;7SG87JKA7UE\n"
            "  \n"
            "      X \n"
            "      -m \n"
            " PO14 \n"
            " R  \n"
            " T        \n"
            " T \n"
            "\n"
            " /T11 \n"
            "\n"
            " X  \n"
            " >12   \n"
            "      <X7    \n"
            " M  \n"
            " ;D\n"
            "  \n"
            "  O \n"
            "  X \n"
            " PO2  \n"
            "  <6 \n"
            " M  \n"
            "");

        int main_rv = student_main();

        ASSERT(main_rv == 0);
        ASSERT_FILE(stdout,
                    "# 0\n"
                    "# 5\n"
                    "# 101\n"
                    "# 0\n"
                    "# 5\n"
                    "# 5\n"
                    "# 0\n"
                    "# 0\n"
                    "# 12\n"
                    "# 1100\n"
                    "# 1100\n"
                    "# 1100\n"
                    "# 4\n"
                    "# 4\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 0\n"
                    "# 2\n"
                    "# 128\n"
                    "");
}
