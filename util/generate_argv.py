import sys
import random

file_name = sys.argv[1]
argv = sys.argv[2:]
argc = len(argv)
assert(argc >= 0)

cookie = ["736051737873545250", "i2c_has_2_wires", "clear_your_event_flags"]
f = open(file_name, "w")
f.truncate()
split_arg = repr(argv).replace('\'', '\"')[1:-1]
autogen_header = "/**************************THIS FILE WAS AUTOGENERATED**************************/\n"
line = '#include <stddef.h>\nstatic char const *user_argv[] = {{\"{}\", {}, NULL}};\nstatic const int user_argc = {};\n'.format(random.choice(cookie), split_arg, str(argc + 1))
autogen_footer = "/*******************************************************************************/"
f.write(autogen_header + line + autogen_footer)
f.close()
