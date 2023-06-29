#include "tau/tau.h"
#include "../src/table.hpp"

TAU_MAIN()

TEST(Table) {
  Table<int> test_table = {};

  TableId item1 = test_table.allocate(42); 
  TableId item2 = test_table.allocate(8086); 

  CHECK_EQ(*test_table.get(item1), 42);
  CHECK_EQ(*test_table.get(item2), 8086);

  test_table.remove(item1);

  CHECK_EQ(test_table.get(item1), nullptr);

  TableId item3 = test_table.allocate(42); 

  CHECK_EQ(test_table.get(item1), nullptr);

  CHECK_EQ(test_table.get({ 2 }), NULL);
}