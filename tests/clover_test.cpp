#include <stdlib.h>
#include <check.h>
#include "../src/clover/clover.h"

START_TEST (test_new_clover) {
  Clover clover = Clover(NULL, NULL);
  Clover * clover_ref = &clover;
  fail_if(clover_ref->get_visualizer() == NULL, "source element is null");
} END_TEST

Suite * new_clover_suite() {
  Suite * s = suite_create("clover");
  TCase * tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_new_clover);

  suite_add_tcase(s, tc_core);
  return s;
}

int main() {
  int num_failed = 0;
  
  Suite * clover_suite = new_clover_suite();
  SRunner * clover_suite_runner = srunner_create(clover_suite);

  srunner_run_all(clover_suite_runner, CK_NORMAL);
  num_failed = srunner_ntests_failed(clover_suite_runner);
  
  srunner_free(clover_suite_runner);
  
  return (num_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}
