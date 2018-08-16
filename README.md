# openMP_Points2Tiles
transform millions of points to tiles as heatmap by openMP
support .csv data
#usage
turn on openMP support and compile in x64 platform
# output result
1. tile data with points(x,y) --[choose],
2. tile pixel data with point count --[final result].
#file format
1. tile data with points：
	|_tilepoints
	   |_level 							//tile level file
		 |_level_row_col.tilepoints		//tile point data file
		 |_level_row_col.tilepoints
		 |_...
		 |_level_row_col.tilepoints
	   |_level
	   |_...
	   |_level
2. tile data with points count:
	|_counttile
	   |_minmax.dat                     //minmax count for each level, can be used to render img
	   |_level 							//tile level file
		 |_level_row_col.counttile		//tile pixel data, format:[pixel position (in resolution*resolution)]#[count]
		 |_level_row_col.counttile
		 |_...
		 |_level_row_col.counttile
	   |_level
	   |_...
	   |_level