# openMP_Points2Tiles
transform millions of points to tiles as heatmap by openMP<br>
<<<<<<< HEAD
support .csv data<br>
about 100k points per second<br>
# usage
turn on openMP support and compile in x64 platform<br>
in VS, release is better
# output result
1. tile data with points(x,y) --[choose],
2. tile pixel data with point count & minmax for each tile(for rendering) --[final result].<br><br>
**Every level has its floder**
=======
support .csv data
# usage
turn on openMP support and compile in x64 platform<br>
# output result
1. tile data with points(x,y) --[choose],<br>
2. tile pixel data with point count --[final result].<br>
# file format
1. tile data with points：<br>
	|_tilepoints<br>
	   |_level 							//tile level file<br><br>
		 |_level_row_col.tilepoints		//tile point data file<br>
		 |_level_row_col.tilepoints<br>
		 |_...<br>
		 |_level_row_col.tilepoints<br>
	   |_level<br>
	   |_...<br>
	   |_level<br>
2. tile data with points count:<br>
	|_counttile<br>
	   |_minmax.dat                     //minmax count for each level, can be used to render img<br>
	   |_level 							//tile level file<br>
		 |_level_row_col.counttile		//tile pixel data, format:[pixel position (in resolution*resolution)]#[count]<br>
		 |_level_row_col.counttile<br>
		 |_...<br>
		 |_level_row_col.counttile<br>
	   |_level<br>
	   |_...<br>
	   |_level<br>
>>>>>>> 1f5154fdb4bef664411e0b846376fef7f4fce763
