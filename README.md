# Soil Settlement
Calculates soil settlement and creates graphs

Examples, binaries and perhaps a UI version coming soon

## Build Requirements
- C Compiler (gcc)
- Make
- Python3
- Python bokeh library (pip3 install bokeh)

## Build Guide
`cd src && make`

## Usage Guide
- Create a file containing all information about the foundation and soils using the following format and save it as `input.txt`:
```
foundation length (x [m]), foundation width (y [m]), foundation depth (z [m]), tension on foundation's lower surface (σ [kN/m²])
number of different soils
soil #1 upper boundary (z [m]), weight (ɣ [kN/m³]), constrained modulus (Eₛ [kN/m²])
soil #2 upper boundary (z [m]), weight (ɣ [kN/m³]), constrained modulus (Eₛ [kN/m²])
soil #3 upper boundary (z [m]), weight (ɣ [kN/m³]), constrained modulus (Eₛ [kN/m²])
...
```
- Run the calculations  
Run `./main -i input.txt -o output.txt`  
The rightmost value in `output.txt`'s last line is the total settlement.
- Create the graphs  
Run `python3 visualization.py`  
Enter the paths to `input.txt` and `output.txt` when prompted  
A webpage will containing the graphs will be opened in your browser, there you can save them as images