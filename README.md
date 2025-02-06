[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)

# Hilbert

A C++ implementation for automotive suspension analysis, developed to experiment with and implement the testing procedures outlined by the _European Garage Equipment Association_ (EGEA).
Using the Hilbert transform for phase-shift analysis between force and displacement signals, this project aims to provide a reliable method for identifying faulty suspension systems during mandatory vehicle inspections.

<div align="center">
	<img src="https://i.imgur.com/HC1QBgP.png" alt="Displacement plot">
</div>



## Background

The European Garage Equipment Association (EGEA) establishes standards and procedures for vehicle testing equipment across Europe.

One critical aspect of vehicle safety inspections is suspension testing. This project explores an innovative approach to suspension analysis by:

1. Measuring the phase relationship between applied force and displacement.
2. Using the Hilbert transform to accurately calculate phase shifts.
3. Providing a reliable method to identify suspension defects.

The implementation serves as both a proof-of-concept and a practical tool for suspension testing procedures aligned with EGEA guidelines.

A summary of the procedure can be viewed [here](https://www.youtube.com/watch?v=4CSZhH3MUGM&t=38s).


## Features

### Physics Simulation
- High-precision RK4 integrator for accurate physical modeling.
- Configurable system parameters (masses, spring constants, damping coefficients).
- Real-time simulation capability.

### Signal Processing
- Fast Fourier Transform (FFT) based Hilbert transform implementation.
- Phase shift analysis between force and platform displacement signals.
- Efficient signal filtering and processing.

### Data Analysis & Visualization
- Export functionality for analysis and plotting.
- Python-based graph visualization suite for simulation results.



## System Modeling

The wheel-suspension-platform system is modelled as follows:

1. The sprung and unsprung masses have a mass of $m_s$ and $m_u$, respectively.
2. The suspension is a damped spring between the sprung and unsprung masses, with parameters $k_s$ and $c_s$.
3. A spring with constant $k_t$ is used to model the tire force when in contact with the platform.

```
  +----------------+
  |   Sprung Mass  | (m_s)
  +----------------+
        |   |       
     [k_s] [c_s]   <-- Suspension spring and damper
        |   |
  +----------------+
  | Unsprung Mass  | (m_u)
  +----------------+
          |           
        [k_t]      <-- Tire spring
          |
      Platform
```

Let $x_s(t)$ and $x_u(t)$ be the displacement of the sprung and unsprung masses, respectively.

Let $y_g(t)$ be platform displacement.

The equations of motion then become:

- Sprung mass:
```math
m_u \ddot x_s + c_s (\dot x_s - \dot x_u) + k_s(x_s - x_u) = 0
```
- Unsprung mass:
```math
m_u \ddot x_u + c_s(\dot x_u - \dot x_s) + k_s(x_u - x_s) + k_t(x_u - y_g) = 0
```



## Dependencies
- FFTW3
- C++23-compatible compiler (GCC 14+, Clang 16+, MSVC 19.38+).



## Usage

### Using Nix

Hilbert is packaged as a Nix Flake:

```bash
nix build
```


### Using Vcpkg

1.  Clone the repository and navigate to its root directory.
2.  Install [vcpkg](https://vcpkg.io/en/) if you don't have it already.
3.  Make sure `VCPKG_ROOT` points to the vcpkg install folder or set `CMAKE_TOOLCHAIN_FILE` to point to `<VCPKG_ROOT>/scripts/buildsystems/vcpkg.cmake"`.
4.  Configure, build and install using CMake:
    
    ```bash
    # List available presets
    cmake --list-presets
    
    # Or set options manually
    cmake -S . -B out -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake

    # Build
    cmake --build out

    # Install
    cmake --build out --target install
    ```



## License

This project is licensed under the [MIT License](LICENSE).
