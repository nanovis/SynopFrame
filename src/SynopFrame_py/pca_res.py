import argparse
from sys import stderr
from json import dump
import pickle
import numpy as np
from oxDNA_analysis_tools.UTILS.RyeReader import get_confs, describe, inbox
from oxDNA_analysis_tools.UTILS.data_structures import TopInfo
from oxDNA_analysis_tools.pca import align_positions
from oxDNA_analysis_tools.UTILS.data_structures import Configuration, TopInfo, TrajInfo


def unpack_pickle(picklefile: str):
    with open(picklefile, 'rb') as handle:
        pca_res = pickle.load(handle)

    coordinates, evalues, evectors, covariation_matrix = pca_res

    return coordinates, evalues, evectors, covariation_matrix


def align_traj(centered_ref_coords:np.ndarray, top_info:TopInfo, traj_info: TrajInfo):
    confs = get_confs(top_info, traj_info, 0, traj_info.nconfs)
    print("test")
    # for conf in :
    # Use oat align
    # https://lorenzo-rovigatti.github.io/oxDNA/oat/cli.html#align





def cli_parser():
    parser = argparse.ArgumentParser(prog="python pca_res.py", description="print PCA coordinates and aligned trajectory")
    parser.add_argument("trajectory", type=str, nargs=1, help="")
    parser.add_argument("meanfile", type=str, nargs=1, help="")
    parser.add_argument("picklefile", type=str, nargs=1, help="")
    parser.add_argument("pca_coords", type=str, nargs=1, help="Output json")

    return parser


def main():
    parser = cli_parser()
    [traj_file], [mean_file], [picklefile], [pca_coords] = vars(parser.parse_args()).values()

    coordinates, evalues, evectors, covariation_matrix = unpack_pickle(picklefile)

    top_info, traj_info = describe(None, traj_file)
    _, mean_info = describe(None, mean_file)

    align_conf = get_confs(top_info, mean_info, 0, 1)[0]
    cms = np.mean(align_conf.positions, axis=0)
    align_conf.positions -= cms
    mean_reconstruction_coord = np.dot(evectors, align_conf.positions.flatten())

    # print(mean_reconstruction_coord[0:3])

    print("INFO: Printing PCA coordinates to {}".format(pca_coords), file=stderr)

    out = {
        "mean": mean_reconstruction_coord[0:3].real.tolist(),
        "data": coordinates[:, 0:3].tolist()
    }

    dump(out, open(pca_coords, 'w'))


if __name__ == '__main__':
    main()



