# SynopFrame
Multiscale Time-dependent Visual Abstraction Framework for Exploratory Analysis of DNA Nanotechnology Simulations

Case study video: https://youtu.be/Hu0tfpS7JBk

Feedback form: https://forms.gle/MH2F7mkDQVHpfNUv6

# Hardware 

Currently only tested on Windows and Mac. A low-end gaming laptop (NVIDIA GeForce GTX 1050 Ti, Intel Core i3-8100 CPU @ 3.60GHz, 16GB RAM) maintains an interactive frame rate. A high-end MacBook Pro (16-inch, 2019, Intel Core i9, 32GB RAM) cannot maintain an interactive frame rate, so we do not recommend users to try SynopFrame on Mac. We believe a Linux machine with a decent graphics card should work well. 

# Installation

Windows installation [guide](https://github.com/nanovis/SynopFrame/blob/main/Install-Win.md). 

For Linux users, the installation process should be similar to Windows. We are happy to guide you through the process. 

# Load your own data

To run SynopFrame for your own dataset, you need to create a folder with the following contents. 

```bash
└─<case_folder_name>
  ├─input
  │  ├─design.oxdna.dat
  │  ├─design.oxdna.top
  │  ├─design.synopspace.hb
  │  ├─mean.oxdna.dat
  │  ├─trajectory_run.synopspace.hb
  │  ├─trajectory_run.synopspace.pca_coords
  │  └─trajectory_run_aligned_to_mean.oxdna.dat
  └─<structure_name>.hipnc # this is copied from e.g. `demo_cube.hipnc`
```

And then open `<structure_name>.hipnc`, go to the `IO_and_Controller` panel, the `IO` tab, change the `print_conf_interval` accordingly. And then click `Cache All`. After the data is cached, you dataset is then at the same state as the demo dataset and is ready to analyze. 

# Feedback and questions 

Please use the GitHub Issues. 


