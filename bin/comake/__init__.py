import os, sys
import logging
import logging.handlers

DEFAULT_BUILD_NAME = 'build.comake.py'
COMAKE_OUTPUT_DIRECTORY = '.comake.build'
LOGGER = logging.getLogger("comake_logger")

def _build_root():
    # TOPDIR
    from os.path import dirname
    return dirname(dirname(dirname(__file__)))

build_root = _build_root()

def main(args):
    LOGGER.setLevel(logging.DEBUG)
    logging.basicConfig(
        format="%(asctime)s - %(levelname)s - %(message)s",
        datefmt="%Y-%m-%d %H:%M:%S",
    )
    os.makedirs(os.getenv('BUILD_DIR'), exist_ok=True)
    file_handler = logging.FileHandler(os.path.join(os.getenv('BUILD_DIR'), "comake.err"), mode="a")
    file_handler.setLevel(logging.ERROR)
    file_handler.setFormatter(logging.Formatter("%(asctime)s - %(levelname)s - %(filename)s:%(lineno)d - %(message)s", datefmt="%Y-%m-%d %H:%M:%S"))
    LOGGER.addHandler(file_handler)

    LOGGER.debug("Enter `%s': %s()", "comake_main", "main")

    os.chdir(build_root)

    if '--help' in args or len(args) == 0:
        LOGGER.warning("make.py [target_name] (--help) (--dump)")
        return

    filename = args[0]
    if filename == 'clean':
        from comake.target import clean
        clean()
    else:
        from comake.target import create_target_tree, statistics
        from comake.target import TargetNotFoundError, BuildCancelError
        from comake.tools import TargetNotBuildError
        LOGGER.info("Analyzing target tree: `%s' ..." % filename)
        try:
            target_tree = create_target_tree(filename)
        except BuildCancelError:
            sys.exit(3)
        except TargetNotFoundError:
            sys.exit(2)

        if '--dump' in args:
            target_tree.dump()
        else:
            LOGGER.info("Starting build")
            try:
                target_tree.build()
            except TargetNotBuildError:
                # Don't Traceback Python scripts for build errors
                LOGGER.error("Error: Target not build (TargetNotBuildError)")
                sys.exit(1)
            if statistics.made_targets == 0:
                LOGGER.info("No targets were rebuilt.")
            LOGGER.info("Total number of targets: %d", statistics.targets)
            if statistics.made_targets != 0:
                LOGGER.info("Targets rebuilt: %d", statistics.made_targets)
