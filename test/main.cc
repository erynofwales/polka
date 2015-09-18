/* main.cc
 * Eryn Wells <eryn@erynwells.me>>
 */

/** Basic driver for unit tests. */

#include "gtest/gtest.h"


int
main(int argc,
     char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
