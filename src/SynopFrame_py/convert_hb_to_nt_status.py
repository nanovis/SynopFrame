import csv
import argparse
import os
import pickle
from array import array

# Some global variables
num_nt = 0
hb_design_dict = {}


def cli_parser(prog="convert_hb_to_nt_status.py"):
    parser = argparse.ArgumentParser(prog=prog, description="convert hb list to nt status")
    parser.add_argument('top', type=str, nargs=1, help="Topology file")
    parser.add_argument('hb_design', type=str, nargs=1, help="The designed hb list")
    parser.add_argument('hb_list', type=str, nargs=1, help="The hb list from the trajectory")
    parser.add_argument('output_file', type=str, nargs=1, help="Name of the file to save the text format output")
    parser.add_argument('-p', '--pickle', type=str, dest='pickle', nargs=1, help='Name of the file to save the pickle format output')
    return parser


def construct_hb_design(hb_design_file: str) -> dict:
    hb_design_dict = {}

    with open(hb_design_file, 'rt') as f:
        csv_read = csv.reader(f, delimiter=' ', skipinitialspace=True)
        # omit the header (the first line)
        next(csv_read)
        for line in csv_read:
            if len(line) == 2:
                # print(line)
                hb_design_dict[int(line[0])] = int(line[1])

    return hb_design_dict


def new_frame() -> list:
    global num_nt
    return [0] * num_nt


def parse_one_frame(reader: csv.reader) -> list:
    hb_frame_list = new_frame()

    for row in reader:
        if len(row) == 0:

            return hb_frame_list

        if len(row) == 2:
            first_nt = int(row[0])
            second_nt = int(row[1])
            if first_nt in hb_design_dict:
                if hb_design_dict[first_nt] == second_nt:
                    # designed to be hb, correct hb
                    hb_frame_list[first_nt] = (2, second_nt)
                    hb_frame_list[second_nt] = (2, first_nt)
                else:
                    # first nt designed to be hb, but wrong partner
                    hb_frame_list[first_nt] = -2
                    if second_nt in hb_design_dict:
                        # second nt designed to be hb, but wrong partner
                        hb_frame_list[second_nt] = -2
                    else:
                        # second nt designed to be single, but paired
                        hb_frame_list[second_nt] = -1

            else: # first nt designed to be single, but paired
                hb_frame_list[first_nt] = -1
                if second_nt in hb_design_dict:
                    # second nt designed to be hb, but wrong partner
                    hb_frame_list[second_nt] = -2
                else:
                    # second nt designed to be single, but paired
                    hb_frame_list[second_nt] = -1

    return hb_frame_list


def parse_hb_list(hb_list_file: str) -> dict:
    hb_traj_list = []

    with open(hb_list_file, 'rt') as f:
        csv_reader = csv.reader(f, delimiter=' ', skipinitialspace=True)
        for row in csv_reader:
            if row[0] == '#':
                hb_frame_list = parse_one_frame(csv_reader)
                
                # check for the nt that are designed to be single, and is single
                # assign 1 to them
                # for all the remaining 0, if they are in the dict, then they remain 0
                # otherwise, they should be 1
                for nt_idx, nt_status in enumerate(hb_frame_list):
                    if nt_status == 0:
                        if nt_idx not in hb_design_dict and nt_idx not in hb_design_dict.values():
                            hb_frame_list[nt_idx] = 1

                frame = int(row[2])
                hb_traj_list.append((frame, hb_frame_list))

    return hb_traj_list


def get_num_nt(top_file: str) -> int:
    with open(top_file, 'rt') as top:
        header = top.readline()

        return int(header.split()[0])


def main():
    parser = cli_parser(os.path.basename(__file__))
    args = parser.parse_args()

    top_file = args.top[0]
    hb_design = args.hb_design[0]
    hb_list = args.hb_list[0]
    output_file = args.output_file[0]

    global hb_design_dict
    hb_design_dict = construct_hb_design(hb_design)

    global num_nt
    num_nt = get_num_nt(top_file)

    hb_traj_list = parse_hb_list(hb_list)

    with open(output_file, 'wt') as out:
        for frame in hb_traj_list:
            print("t = " + str(frame[0]), file=out)
            for item in frame[1]:
                if isinstance(item, int):
                    print(item, file=out)
                else:
                    print(item[0], item[1], file=out)


    if args.pickle:
        pickle_file = args.pickle[0]
        with open(pickle_file, 'wb') as out_pickle:
            pickle.dump(hb_traj_list, file=out_pickle, protocol=pickle.HIGHEST_PROTOCOL)


if __name__ == '__main__':
    main()
