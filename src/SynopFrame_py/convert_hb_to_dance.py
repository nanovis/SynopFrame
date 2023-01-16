import argparse
import os
from collections import namedtuple


def cli_parser(prog="convert_hb_to_dance.py"):
    parser = argparse.ArgumentParser(prog = prog, description="convert hb list to dance file")
    parser.add_argument("top", type=str, nargs=1, help="Topology file")
    parser.add_argument("hb_design", type=str, nargs=1, help="The hb design")
    parser.add_argument("hb_list", type=str, nargs=1, help="The hb list from the trajectory")
    parser.add_argument("output_file", type=str, nargs=1, help="Name of the dance file to output")

    return parser


NT = namedtuple("NT", "strand_id base_type end3 end5 id_on_strand")
Strand = namedtuple("Strand", "length sequence")


def get_nt_dict(top_file: str) -> dict:
    nt_dict = {}

    with open(top_file, 'rt') as top:
        top.readline()

        nt_id = 0
        line = top.readline()
        while line:
            tokens = line.split()

            # put the NT to dict
            strand_id = int(tokens[0])
            base_type = tokens[1]
            end3 = int(tokens[2])
            end5 = int(tokens[3])

            # id_on_strand default to -2
            # because -1 is used in end3 and end5 as end indication
            nt_dict[nt_id] = NT(strand_id, base_type, end3, end5, -2)

            nt_id += 1
            line = top.readline()

    return nt_dict


def change_nt_id_on_strand(nt: NT, id_on_strand: int) -> NT:
    nt = NT(nt.strand_id, nt.base_type, nt.end3, nt.end5, id_on_strand)
    return nt

def get_strand_dict(nt_dict: dict) -> dict:
    visited_nt = set()
    strand_dict = {}

    for nt in nt_dict:
        if nt not in visited_nt:
            # Find end3
            # This requires non-circular topology
            strand_head_id = nt
            end3 = nt_dict[strand_head_id].end3

            # If the current nt's end3 is not -1,
            # Make the nt's end3 the strand_head_id and check again
            while end3 != -1:
                strand_head_id = end3
                end3 = nt_dict[strand_head_id].end3

            # Now we have the strand_head_id
            # We trace end5 to get the whole sequence

            strand_dict[nt_dict[strand_head_id].strand_id] = Strand(1, nt_dict[strand_head_id].base_type)
            nt_dict[strand_head_id] = change_nt_id_on_strand(nt_dict[strand_head_id], 0)
            visited_nt.add(strand_head_id)
            end5 = nt_dict[strand_head_id].end5
            while end5 != -1:
                strand_head_id = end5
                new_len = strand_dict[nt_dict[strand_head_id].strand_id].length + 1
                new_seq = strand_dict[nt_dict[strand_head_id].strand_id].sequence + nt_dict[strand_head_id].base_type
                nt_dict[strand_head_id] = change_nt_id_on_strand(nt_dict[strand_head_id], new_len - 1)  # -1 is because this index is 0-based
                strand_dict[nt_dict[strand_head_id].strand_id] = Strand(new_len, new_seq)
                visited_nt.add(strand_head_id)
                end5 = nt_dict[strand_head_id].end5

    return strand_dict


def parse_hb_design(hb_design: str, nt_dict: dict) -> list:
    hb_design_str_list = ["## Target_bonding_configuration\n"]

    with open(hb_design, 'rt') as hb_handle:
        line = hb_handle.readline()
        line = hb_handle.readline()
        while line:
            nt_1, nt_2 = [int(i) for i in line.split()]
            nt_1_str = str(nt_dict[nt_1].strand_id) + ".1." + str(nt_dict[nt_1].id_on_strand)
            nt_2_str = str(nt_dict[nt_2].strand_id) + ".1." + str(nt_dict[nt_2].id_on_strand)
            hb_design_str_list.append(nt_1_str + "\t" + nt_2_str + "\n")
            line = hb_handle.readline()

    hb_design_str_list.append("\n")

    return hb_design_str_list


def parse_hb_list(hb_list: str, nt_dict: dict, strand_dict: dict) -> [list, list]:
    header_str_list = []
    hb_dance_str_list = ["## Bonding_info\n\n"]

    current_step = 0
    print_conf_interval = 0

    with open(hb_list, 'rt') as hb_handle:
        line = hb_handle.readline()
        while line:
            if line.startswith("#"):
                hb_dance_str_list.append(line)

                # get print_conf_interval
                # sanity check: all print_conf_interval should be the same
                if print_conf_interval == 0 and current_step > 0:
                    print_conf_interval = int(line.split()[2]) - current_step
                elif print_conf_interval > 0:
                    if not int(line.split()[2]) - current_step == print_conf_interval:
                        raise Exception("print_conf_interval inconsistent at:", line.split()[2])

                current_step = int(line.split()[2])
                line = hb_handle.readline()
            elif line.startswith("\n"):
                hb_dance_str_list.append("\n")
                line = hb_handle.readline()
            elif line[0].isdigit():
                nt_1, nt_2 = [int(i) for i in line.split()]
                nt_1_str = str(nt_dict[nt_1].strand_id) + ".1." + str(nt_dict[nt_1].id_on_strand)
                nt_2_str = str(nt_dict[nt_2].strand_id) + ".1." + str(nt_dict[nt_2].id_on_strand)
                hb_dance_str_list.append(nt_1_str + "\t" + nt_2_str + "\n")
                line = hb_handle.readline()
            else:
                raise Exception("Unexpected line:", line)

    # add template
    header_str_list.append("## Header_for_metadata\n")
    header_str_list.append("# one_step_in_physical_time = 3.03 ps\n")
    header_str_list.append("# steps = " + str(current_step) + "\n")
    header_str_list.append("# print_conf_interval = " + str(print_conf_interval) + "\n")
    header_str_list.append("## strand_sequence_start\n")

    # add sequence
    for i in range(1, len(strand_dict) + 1):
        header_str_list.append("# " + str(i) + ":1:" + str(strand_dict[i].length) + ":" + strand_dict[i].sequence + "\n")

    header_str_list.append("## strand_sequence_stop\n")
    header_str_list.append("\n")

    return header_str_list, hb_dance_str_list


def main():
    parser = cli_parser(os.path.basename(__file__))
    args = parser.parse_args()
    top_file = args.top[0]
    hb_design = args.hb_design[0]
    hb_list = args.hb_list[0]
    output_file = args.output_file[0]

    nt_dict = get_nt_dict(top_file)
    strand_dict = get_strand_dict(nt_dict)

    # parse hb_list
    header_str_list, hb_dance_str_list = parse_hb_list(hb_list, nt_dict, strand_dict)

    hb_design_str_list = parse_hb_design(hb_design, nt_dict)

    with open(output_file, 'wt') as out_handle:
        out_handle.write("".join(header_str_list))
        out_handle.write("".join(hb_design_str_list))
        out_handle.write("".join(hb_dance_str_list))
    print("Done!\n===\n")


if __name__ == '__main__':
    main()
