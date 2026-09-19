#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

static int int_param = 0;
module_param(int_param, int, 0644);
MODULE_PARM_DESC(int_param, "declared int param");

static bool bool_param = false;
module_param(bool_param, bool, 0644);
MODULE_PARM_DESC(bool_param, "declared bool param");

static char *charp_param = "default_charp";
module_param(charp_param, charp, 0644);
MODULE_PARM_DESC(charp_param, "declared charp param");

static char string_param[32] = "default_string";
module_param_string(string_param, string_param, sizeof(string_param), 0644);
MODULE_PARM_DESC(string_param, "declared string param");

static int __init module_param_init(void) {
  pr_info("module-param: module loaded\n");
  pr_info("module-param: int_param = %d\n", int_param);
  pr_info("module-param: bool_param = %s\n", bool_param ? "true" : "false");
  pr_info("module-param: charp_param = %s\n", charp_param);
  pr_info("module-param: string_param = %s\n", string_param);
  return 0;
}

static void __exit module_param_exit(void) {
  pr_info("module-param: module unloaded\n");
}

module_init(module_param_init);
module_exit(module_param_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hsiu-Chi Chang");
MODULE_DESCRIPTION("Practice module parameters");