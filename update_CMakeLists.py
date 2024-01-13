#!/usr/bin/env python3

from pathlib import Path
import sys
from tempfile import TemporaryFile
from enum import Enum, auto

LIB_SRC_START_MARKER = "# lib src start"
LIB_SRC_END_MARKER = "# lib src end"
EXE_SRC_START_MARKER = "# exe src start"
EXE_SRC_END_MARKER = "# exe src end"


class CMakeTargetType(Enum):
    LIB = auto()
    EXE = auto()
    DEFAULT = auto()


def main():
    assert len(sys.argv) == 2
    target = Path(sys.argv[1])
    assert target.is_file
    print(f"updating {target}")
    target_dir = target.parent
    existing_sources = [path.relative_to(target_dir) for path in target_dir.glob("*.cpp")]
    target_type = CMakeTargetType.DEFAULT
    is_in_add_func = False
    is_in_src = False
    with TemporaryFile("w+", encoding="utf8") as tmpfile:
        with open(target, "r", encoding="utf8") as infile:
            for line in infile:
                if "add_executable" in line.lower():
                    target_type = CMakeTargetType.EXE
                    is_in_add_func = True
                    tmpfile.write(line)
                    continue
                if "add_library" in line.lower():
                    target_type = CMakeTargetType.LIB
                    is_in_add_func = True
                    tmpfile.write(line)
                    continue
                if is_in_add_func:
                    if (target_type == CMakeTargetType.LIB and LIB_SRC_START_MARKER in line.lstrip()) or (
                        target_type == CMakeTargetType.EXE and EXE_SRC_START_MARKER in line.lstrip()
                    ):
                        is_in_src = True
                        tmpfile.write(line)
                        continue
                    if (target_type == CMakeTargetType.LIB and LIB_SRC_END_MARKER in line.lstrip()) or (
                        target_type == CMakeTargetType.EXE and EXE_SRC_END_MARKER in line.lstrip()
                    ):
                        for source in existing_sources:
                            tmpfile.write(f"{source}\n")
                        is_in_src = False
                        is_in_add_func = False
                        tmpfile.write(line)
                        continue
                    if is_in_src:
                        if len(line.lstrip()) == 0 or line.lstrip()[0] == "#":
                            tmpfile.write(line)
                            continue
                        source = Path(line.split("#")[0].strip())
                        if source not in existing_sources:
                            ans = input(f"file '{source}' wasn't found. preserve it? [Y/n]")
                            ans = ans.strip().lower()
                            if ans == "n" or ans == "no":
                                print(f"reference to '{source}' is removed from CMakeLists.txt")
                                continue
                            else:
                                print(f"reference to '{source}' is kept in CMakeLists.txt")
                                tmpfile.write(line)
                                continue
                        existing_sources.remove(source)
                        tmpfile.write(line)
                        continue
                tmpfile.write(line)
                continue
        tmpfile.seek(0)
        with open(target, "w", encoding="utf8") as outfile:  # 上書き
            outfile.write(tmpfile.read())


if __name__ == "__main__":
    main()
