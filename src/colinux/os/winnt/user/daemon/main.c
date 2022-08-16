#include <stdio.h>

#include <colinux/user/manager.h>
#include <colinux/user/debug.h>

static co_rc_t co_winnt_main(int argc, char *args[])
{
	co_rc_t rc = CO_RC_OK;
	co_winnt_parameters_t winnt_parameters;
	co_start_parameters_t start_parameters;
	co_command_line_params_t cmdline;

	co_memset(&start_parameters, 0, sizeof(start_parameters));
	co_memset(&winnt_parameters, 0, sizeof(winnt_parameters));

	co_daemon_print_header();

	co_winnt_change_directory_for_service(argc, args);

	rc = co_cmdline_params_alloc(args, argc, &cmdline);
	if (!CO_OK(rc)) {
		co_terminal_print("daemon: error parsing arguments\n");
		return rc;
	}

	rc = co_winnt_daemon_parse_args(cmdline, &winnt_parameters);
	if (!CO_OK(rc)) {
		co_terminal_print("daemon: error parsing parameters\n");
		co_winnt_help();
		return CO_RC(ERROR);
	}

	rc = co_daemon_parse_args(cmdline, &start_parameters);
	if (!CO_OK(rc) || start_parameters.show_help){
		if (!CO_OK(rc)) {
			co_terminal_print("daemon: error parsing parameters\n");
		}
		co_winnt_help();
		return CO_RC(ERROR);
	}

	rc = co_cmdline_params_check_for_no_unparsed_parameters(cmdline, PFALSE);
	if (!CO_OK(rc)) {
		co_winnt_help();
		co_terminal_print("\n");
		co_cmdline_params_check_for_no_unparsed_parameters(cmdline, PTRUE);
		return CO_RC(ERROR);
	}

	if (winnt_parameters.status_driver) {
		return co_winnt_status_driver(1); // arg 1 = View all driver details
	}

	if (winnt_parameters.install_driver) {
		rc = co_winnt_install_driver();
		if (CO_OK(rc)) {
			co_terminal_print("daemon: driver installed\n");
		}
		return rc;
	}

	if (winnt_parameters.install_service) {
		char *szCmdLine, *p, **pp;
		int size;

		if (!start_parameters.config_specified) {
			co_terminal_print("daemon: config not specified\n");
			return CO_RC(ERROR);
		}

		// Create single command line back
		for (size = 0, pp = args; *pp; pp++)
			size += 1+strlen(*pp);
		szCmdLine = malloc(size);
		for (p = szCmdLine, pp = args; *pp; pp++)
			p += snprintf(p, size - (p - szCmdLine), (p == szCmdLine) ? "%s" : " %s", *pp);

		return co_winnt_daemon_install_as_service(winnt_parameters.service_name,
							  szCmdLine,
							  start_parameters.network_types);
	}

	if (winnt_parameters.remove_service) {
		return co_winnt_daemon_remove_service(winnt_parameters.service_name);
	}

	if (winnt_parameters.remove_driver) {
		return co_winnt_remove_driver();
	}

	if (winnt_parameters.run_service) {
		co_running_as_service = PTRUE;

		co_terminal_print("colinux: running as service '%s'\n",
				  winnt_parameters.service_name);
		if (start_parameters.launch_console)
		{
			co_terminal_print("colinux: not spawning a console, because we're running as service.\n");
			start_parameters.launch_console = PFALSE;
		}

		return co_winnt_daemon_initialize_service(&start_parameters);
	}

	if (!start_parameters.config_specified){
		if (!start_parameters.cmdline_config) {
			co_daemon_syntax();
			co_winnt_daemon_syntax();
		}
		return CO_RC(ERROR);
	}

	return co_winnt_daemon_main(&start_parameters);
}

int main(int argc, char *argv[]) {
    co_rc_t rc;
    int ret;

    co_debug_start();

    rc = co_winnt_main(argc-1, argv+1);

    ret = CO_RC_GET_CODE(rc);

    switch (ret) {
    case CO_RC_OK:
    case CO_RC_VERSION_MISMATCHED:
    case CO_RC_ERROR_ACCESSING_DRIVER:
        break;
    default:
        ret = 1;
    }

    co_debug("rc=%08x exit=%d", (int)rc, -ret);

    co_debug_end();

    return -ret;
}
