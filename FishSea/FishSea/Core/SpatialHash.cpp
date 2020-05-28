#include "SpatialHash.h"

SpatialHash::SpatialHash() : CELL_SIZE(60)
{}

SpatialHash::SpatialHash(float wWidth, float wHeight, int cellSize) : CELL_SIZE(cellSize), worldWidth(wWidth), worldHeight(wHeight)
{}

void SpatialHash::add(Entity &e)
{
	int x = (int)e.GetCoordinate().x / CELL_SIZE;
	int y = (int)e.GetCoordinate().y / CELL_SIZE;

	int xL = (int)(e.GetCoordinate().x + e.GetLength().x) / CELL_SIZE;
	int yL = (int)(e.GetCoordinate().y + e.GetLength().y) / CELL_SIZE;

	GridCoordinates coords0(x, y); 
	GridCoordinates coords1(x, yL);
	GridCoordinates coords2(xL, yL);
	GridCoordinates coords3(xL, y);

	std::vector<GridCoordinates> gridcoords;

	
	for (int newx = x; newx <= xL; newx++)
	{
		for (int newy = y; newy <= yL; newy++)
		{
			gridcoords.push_back(GridCoordinates(newx, newy));
		}
	}
	e.SetGrids(gridcoords);
	for (int newx=x;newx <= xL; newx++)
	{
		for (int newy = y; newy <= yL; newy++)
		{			
			grid[GridCoordinates(newx, newy)].push_back(e);
		}
	}
		
	
}


void SpatialHash::clear()
{
	for (auto cell : grid)
	{
		auto bucket = cell.second;
		bucket.clear();
	}
	grid = Grid();
}

void SpatialHash::remove(Entity e)
{
std::vector<GridCoordinates> gridcoords = e.GetGrids();

	for (auto coords : gridcoords)
	{
		grid[coords].erase(std::remove(grid[coords].begin(), grid[coords].end(), e), grid[coords].end());
	}
}

std::vector<Entity> SpatialHash::collect(float camX, float camXWidth, float camY, float camYHeight)
{

	const int c = 150; //Padding so that objects don't pop


	int x = ((int)camX -c)/ CELL_SIZE; if (x < 0) x = 0;
	int y = ((int)camY -c) / CELL_SIZE; if (y < 0) y = 0;

	int xL = ((int)camXWidth +c)/ CELL_SIZE; if (xL < 0) xL = 0;
	int yL = ((int)camYHeight +c) / CELL_SIZE; if (yL < 0) yL = 0;

	std::vector<Entity> objects;
	

	for (int newx=x;newx <= xL;newx++)
	{
		for(int newy=y;newy<=yL;newy++)
		{
			auto object = grid[GridCoordinates(newx, newy)];
			objects.insert(objects.end(),object.begin(),object.end());
		}
	}

	return objects;
}

