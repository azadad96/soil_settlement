import csv
import math
import bokeh
from bokeh.plotting import figure
from bokeh.io import show
from bokeh.models import Span

def main():
	filepath = input("output: ")
	data = []
	with open(filepath, "r") as file:
		reader = csv.reader(file)
		for k, i in enumerate(reader):
			row = []
			for j in range(len(i)):
				row.append(float(i[j]) if i[j] != "" else 0)
			data.append(row)

	filepath = input("input: ")
	soils = []
	foundation_depth = 0
	with open(filepath, "r") as file:
		file_content = file.read()
		foundation_depth = float(file_content.split("\n")[0].replace(" ", "").split(",")[2])
		soils = [[float(j) for j in i.replace(" ", "").split(",")] for i in file_content.split("\n")[2:]]

	bokeh.io.curdoc().theme = "dark_minimal"

	p1 = figure(
		x_axis_label = "Spannung [kN/m²]",
		y_axis_label = "Tiefe [m]",
		x_axis_type = "auto",
		width = 750, height = 900,
		title = "Spannungsverteilung"
	)
	x, y = to_xy(data, 5, 3)
	y = [-i for i in y]
	p1.line(x, y, legend_label = "σQ", line_color = "lightblue", line_width = 2)
	x, y = to_xy(data, 6, 3)
	y = [-i for i in y]
	p1.line(x, y, legend_label = "σ0", line_color = "lightgreen", line_width = 2)
	x, y = to_xy(data, 7, 3)
	y = [-i for i in y]
	p1.line(x, y, legend_label = "0.2 * σ0", line_color = "pink", line_width = 2)
	for i in soils:
		p1.ray(x = [0], y = [-i[0]], length = 0, angle = 0, line_color = "grey", line_dash = "dashed")
		p1.ray(x = [0], y = [-i[0]], length = 0, angle = math.pi, line_color = "grey", line_dash = "dashed")
	p1.ray(x = [0], y = [-foundation_depth], length = 0, angle = 0, line_color = "white", line_dash = "dashed")
	p1.ray(x = [0], y = [-foundation_depth], length = 0, angle = math.pi, line_color = "white", line_dash = "dashed")
	p1.legend.location = "bottom_right"

	p2 = figure(
		x_axis_label = "Setzung [‰]",
		y_axis_label = "Tiefe [m]",
		x_axis_type = "auto",
		width = 1500, height = 900,
		title = "Setzungen"
	)
	x, y = to_xy(data, 9, 3)
	x = [1000 * i for i in x]
	y = [-i for i in y]
	del(x[-1])
	del(y[-1])
	p2.line(x, y, line_color = "white", line_width = 2)
	for i in soils:
		p2.ray(x = [0], y = [-i[0]], length = 0, angle = 0, line_color = "grey", line_dash = "dashed")
		p2.ray(x = [0], y = [-i[0]], length = 0, angle = math.pi, line_color = "grey", line_dash = "dashed")
	p2.ray(x = [0], y = [-foundation_depth], length = 0, angle = 0, line_color = "white", line_dash = "dashed")
	p2.ray(x = [0], y = [-foundation_depth], length = 0, angle = math.pi, line_color = "white", line_dash = "dashed")

	p3 = figure(
		x_axis_label = "Setzung [cm]",
		y_axis_label = "Tiefe [m]",
		x_axis_type = "auto",
		width = 1500, height = 900,
		title = "Setzungen"
	)
	x, y = to_xy(data, 10, 3)
	x = [100 * i for i in x]
	for i in range(1, len(x)):
		x[i] += x[i - 1]
	y = [-i for i in y]
	del(x[-1])
	del(y[-1])
	p3.line(x, y, line_color = "white", line_width = 2)
	for i in soils:
		p3.ray(x = [0], y = [-i[0]], length = 0, angle = 0, line_color = "grey", line_dash = "dashed")
		p3.ray(x = [0], y = [-i[0]], length = 0, angle = math.pi, line_color = "grey", line_dash = "dashed")
	p3.ray(x = [0], y = [-foundation_depth], length = 0, angle = 0, line_color = "white", line_dash = "dashed")
	p3.ray(x = [0], y = [-foundation_depth], length = 0, angle = math.pi, line_color = "white", line_dash = "dashed")

	show(bokeh.layouts.column(p1, p2, p3))

def to_xy(data, ix, iy):
	x = []
	y = []
	for i in data:
		x.append(i[ix])
		y.append(i[iy])
	return [x, y]

if __name__ == "__main__":
	main()