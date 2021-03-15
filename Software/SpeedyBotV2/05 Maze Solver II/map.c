/*
 * flood.c
 *
 * Created: 10.02.2020 07:19:29
 *  Author: mschr
 */ 

#include "map.h"
#include "system.h"
#include "uart.h"
#include "maze.h"
#include "stack.h"


direction_t bot_orientation;

position_t bot_position;

uint8_t distance_counter = 0;

tile_t maze[MAZE_X][MAZE_Y];

struct Stack *searchStack;

stateMaze_t mazeState;

bool goalBranch = false;
int8_t goalBranch_Position = 0;

void maze_init(void)
{
	bot_orientation = NORTH;
	bot_position.x = 0;
	bot_position.y = 0;
	
	searchStack = newStack(STACK_SIZE);
	
	mazeState = MAZE_SEARCH;
	
	distance_counter = 0;
	
	for (uint8_t i=0; i<MAZE_X; i++)
	{
		for (uint8_t j=0; j<MAZE_Y; j++)
		{
			maze[i][j].byte = 0;
			maze[i][j].distance = -1;
		}
	}
	
	// Außenwände eintragen
	for (uint8_t i=0; i<MAZE_X; i++)
	{
		maze[i][0].wall_S = true;
		maze[i][MAZE_Y-1].wall_N = true;
	}
	for (uint8_t i=0; i<MAZE_Y; i++)
	{
		maze[0][i].wall_W = true;
		maze[MAZE_X-1][i].wall_E = true;
	}
	
	// Start Point
	maze[0][0].start = true;
	maze[0][0].wall_E = true;
	maze[0][0].wall_S = true;
	maze[0][0].wall_W = true;
	
	//maze[GOAL_X][GOAL_Y].goal = true;
	
}


direction_t get_new_orientation(position_t cell_destination)
{
	direction_t turn_bot = DIR_ERROR;
	
	
	if (bot_position.y+1 == cell_destination.y)
		turn_bot = NORTH;
	else if (bot_position.y-1 == cell_destination.y)
		turn_bot = SOUTH;
	else if (bot_position.x+1 == cell_destination.x)
		turn_bot = EAST;
	else if (bot_position.x-1 == cell_destination.x)
		turn_bot = WEST;
	
	return turn_bot;
}

uint8_t go_Cell(position_t destination)
{
	uint8_t move_cmd = 0;
	direction_t new_orientation;
	
	new_orientation = get_new_orientation(destination);
	
	if (new_orientation == bot_orientation)
		move_cmd = STRAIGHT;
	else if (bot_orientation == NORTH && new_orientation == SOUTH)
		move_cmd = U_TURN;
	else if (bot_orientation == NORTH && new_orientation == WEST)
		move_cmd = LEFT_TURN;
	else if (bot_orientation == NORTH && new_orientation == EAST)
		move_cmd = RIGHT_TURN;
	else if (bot_orientation == WEST && new_orientation == EAST)
		move_cmd = U_TURN;
	else if (bot_orientation == WEST && new_orientation == SOUTH)
		move_cmd = LEFT_TURN;
	else if (bot_orientation == WEST && new_orientation == NORTH)
		move_cmd = RIGHT_TURN;
	else if (bot_orientation == SOUTH && new_orientation == NORTH)
		move_cmd = U_TURN;
	else if (bot_orientation == SOUTH && new_orientation == EAST)
		move_cmd = LEFT_TURN;
	else if (bot_orientation == SOUTH && new_orientation == WEST)
		move_cmd = RIGHT_TURN;
	else if (bot_orientation == EAST && new_orientation == WEST)
		move_cmd = U_TURN;
	else if (bot_orientation == EAST && new_orientation == NORTH)
		move_cmd = LEFT_TURN;
	else if (bot_orientation == EAST && new_orientation == SOUTH)
		move_cmd = RIGHT_TURN;
	
	bot_orientation = new_orientation;
	bot_position = destination;
	
	return move_cmd;
}


position_t go_orientation(uint8_t direction)
{
	position_t new_position;
	new_position.x = bot_position.x;
	new_position.y = bot_position.y;
	
	switch(direction)
	{
		case AHEAD:
		if		(bot_orientation == NORTH)	new_position.y = bot_position.y+1;
		else if (bot_orientation == SOUTH)	new_position.y = bot_position.y-1;	
		else if (bot_orientation == EAST)	new_position.x = bot_position.x+1;
		else if (bot_orientation == WEST)	new_position.x = bot_position.x-1;
		break;
		
		case RIGHT:
		if		(bot_orientation == NORTH)	new_position.x = bot_position.x+1;
		else if (bot_orientation == SOUTH)	new_position.x = bot_position.x-1;
		else if (bot_orientation == EAST)	new_position.y = bot_position.y-1;
		else if (bot_orientation == WEST)	new_position.y = bot_position.y+1;
		break;
		
		case LEFT:
		if		(bot_orientation == NORTH)	new_position.x = bot_position.x-1;
		else if (bot_orientation == SOUTH)	new_position.x = bot_position.x+1;
		else if (bot_orientation == EAST)	new_position.y = bot_position.y+1;
		else if (bot_orientation == WEST)	new_position.y = bot_position.y-1;
		break;
		
		case BACKWARD:
		if		(bot_orientation == NORTH)	new_position.y = bot_position.y-1;
		else if (bot_orientation == SOUTH)	new_position.y = bot_position.y+1;
		else if (bot_orientation == EAST)	new_position.x = bot_position.x-1;
		else if (bot_orientation == WEST)	new_position.x = bot_position.x+1;
		break;
	}
	
	return new_position;
}


void store_actual_walls(intersection_t intersection)
{
	switch(bot_orientation)
	{
		case NORTH:
		if ( intersection == I_DEAD_END )
		{
			maze[bot_position.x][bot_position.y].wall_N = true;
			maze[bot_position.x][bot_position.y].wall_E = true;
			maze[bot_position.x][bot_position.y].wall_W = true;
		}
		else if (intersection == I_STRAIGHT)
		{
			maze[bot_position.x][bot_position.y].wall_E = true;
			maze[bot_position.x][bot_position.y].wall_W = true;
		}
		else if (intersection == I_LEFT)
		{
			maze[bot_position.x][bot_position.y].wall_N = true;
			maze[bot_position.x][bot_position.y].wall_E = true;
		}
		else if (intersection == I_RIGHT)
		{
			maze[bot_position.x][bot_position.y].wall_N = true;
			maze[bot_position.x][bot_position.y].wall_W = true;
		}
		else if (intersection == I_STRAIGHT_LEFT)
		{
			maze[bot_position.x][bot_position.y].wall_W = true;
		}
		else if (intersection == I_STRAIGHT_RIGHT)
		{
			maze[bot_position.x][bot_position.y].wall_W = true;
		}
		else if (intersection == I_T_CROSS)
		{
			maze[bot_position.x][bot_position.y].wall_N = true;
		}
		else if (intersection == I_START)
		{
			maze[bot_position.x][bot_position.y].wall_S = true;
			maze[bot_position.x][bot_position.y].wall_E = true;
			maze[bot_position.x][bot_position.y].wall_W = true;
		}
		break;
		
		case SOUTH:
		if ( intersection == I_DEAD_END )
		{
			maze[bot_position.x][bot_position.y].wall_S = true;
			maze[bot_position.x][bot_position.y].wall_E = true;
			maze[bot_position.x][bot_position.y].wall_W = true;
		}
		else if (intersection == I_STRAIGHT)
		{
			maze[bot_position.x][bot_position.y].wall_E = true;
			maze[bot_position.x][bot_position.y].wall_W = true;
		}
		else if (intersection == I_LEFT)
		{
			maze[bot_position.x][bot_position.y].wall_S = true;
			maze[bot_position.x][bot_position.y].wall_W = true;
		}
		else if (intersection == I_RIGHT)
		{
			maze[bot_position.x][bot_position.y].wall_S = true;
			maze[bot_position.x][bot_position.y].wall_E = true;
		}
		else if (intersection == I_STRAIGHT_LEFT)
		{
			maze[bot_position.x][bot_position.y].wall_W = true;
		}
		else if (intersection == I_STRAIGHT_RIGHT)
		{
			maze[bot_position.x][bot_position.y].wall_E = true;
		}
		else if (intersection == I_T_CROSS)
		{
			maze[bot_position.x][bot_position.y].wall_S = true;
		}
		break;
		
		case EAST:
		if ( intersection == I_DEAD_END )
		{
			maze[bot_position.x][bot_position.y].wall_N = true;
			maze[bot_position.x][bot_position.y].wall_E = true;
			maze[bot_position.x][bot_position.y].wall_S = true;
		}
		else if (intersection == I_STRAIGHT)
		{
			maze[bot_position.x][bot_position.y].wall_N = true;
			maze[bot_position.x][bot_position.y].wall_S = true;
		}
		else if (intersection == I_LEFT)
		{
			maze[bot_position.x][bot_position.y].wall_S = true;
			maze[bot_position.x][bot_position.y].wall_E = true;
		}
		else if (intersection == I_RIGHT)
		{
			maze[bot_position.x][bot_position.y].wall_N = true;
			maze[bot_position.x][bot_position.y].wall_E = true;
		}
		else if (intersection == I_STRAIGHT_LEFT)
		{
			maze[bot_position.x][bot_position.y].wall_S = true;
		}
		else if (intersection == I_STRAIGHT_RIGHT)
		{
			maze[bot_position.x][bot_position.y].wall_N = true;
		}
		else if (intersection == I_T_CROSS)
		{
			maze[bot_position.x][bot_position.y].wall_E = true;
		}
		break;
		
		case WEST:
		if ( intersection == I_DEAD_END )
		{
			maze[bot_position.x][bot_position.y].wall_N = true;
			maze[bot_position.x][bot_position.y].wall_S = true;
			maze[bot_position.x][bot_position.y].wall_W = true;
		}
		else if (intersection == I_STRAIGHT)
		{
			maze[bot_position.x][bot_position.y].wall_N = true;
			maze[bot_position.x][bot_position.y].wall_S = true;
		}
		else if (intersection == I_LEFT)
		{
			maze[bot_position.x][bot_position.y].wall_N = true;
			maze[bot_position.x][bot_position.y].wall_W = true;
		}
		else if (intersection == I_RIGHT)
		{
			maze[bot_position.x][bot_position.y].wall_S = true;
			maze[bot_position.x][bot_position.y].wall_W = true;
		}
		else if (intersection == I_STRAIGHT_LEFT)
		{
			maze[bot_position.x][bot_position.y].wall_N = true;
		}
		else if (intersection == I_STRAIGHT_RIGHT)
		{
			maze[bot_position.x][bot_position.y].wall_S = true;
		}
		else if (intersection == I_T_CROSS)
		{
			maze[bot_position.x][bot_position.y].wall_W = true;
		}
		break;
		
		case DIR_ERROR:
	
		break;
	}
}

direction_t orentationToDirection(uint8_t orientation)
{
	direction_t newDirection = DIR_ERROR;
	
	switch(orientation)
	{
		case AHEAD:
		if (bot_orientation == NORTH)
			newDirection = NORTH;
		else if (bot_orientation == SOUTH)
			newDirection = SOUTH;
		else if (bot_orientation == EAST)
			newDirection = EAST;
		else if (bot_orientation == WEST)
			newDirection = WEST;	
		break;
		
		case BACKWARD:
		if (bot_orientation == NORTH)
			newDirection = SOUTH;
		else if (bot_orientation == SOUTH)
			newDirection = NORTH;
		else if (bot_orientation == EAST)
			newDirection = WEST;
		else if (bot_orientation == WEST)
			newDirection = EAST;
		break;
		
		case LEFT:
		if (bot_orientation == NORTH)
			newDirection = WEST;
		else if (bot_orientation == SOUTH)
			newDirection = EAST;
		else if (bot_orientation == EAST)
			newDirection = NORTH;
		else if (bot_orientation == WEST)
			newDirection = SOUTH;
		break;
		
		case RIGHT:
		if (bot_orientation == NORTH)
			newDirection = EAST;
		else if (bot_orientation == SOUTH)
			newDirection = WEST;
		else if (bot_orientation == EAST)
			newDirection = SOUTH;
		else if (bot_orientation == WEST)
			newDirection = NORTH;
		break;
	}
	
	return newDirection;
}

bool cellVisited(direction_t direction)
{
	bool visited = false;
	
	switch(direction)
	{
		case NORTH:
		if (maze[bot_position.x][bot_position.y+1].visited == true)
			visited = true;
		break;
		
		case WEST:
		if (maze[bot_position.x-1][bot_position.y].visited == true)
			visited = true;
		break;
		
		case EAST:
		if (maze[bot_position.x+1][bot_position.y].visited == true)
			visited = true;
		break;
		
		case SOUTH:
		if (maze[bot_position.x][bot_position.y-1].visited == true)
			visited = true;
		break;
		case DIR_ERROR:
			visited = true;
		break;
	}
	
	return visited;
}

bool cellPassable(direction_t direction)
{
		bool passable = true;
		
		switch(direction)
		{
			case NORTH:
			if (maze[bot_position.x][bot_position.y].wall_N == true)
			passable = false;
			break;
			
			case WEST:
			if (maze[bot_position.x][bot_position.y].wall_W == true)
			passable = false;
			break;
			
			case EAST:
			if (maze[bot_position.x][bot_position.y].wall_E == true)
			passable = false;
			break;
			
			case SOUTH:
			if (maze[bot_position.x][bot_position.y].wall_S == true)
			passable = false;
			break;
			
			case DIR_ERROR:
			passable = false;
			break;
		}
		
		return passable;
}

/*
uint8_t go_Direction(direction_t nextDirection)
{
	uint8_t moveCommand = 0;

	switch(nextDirection)
	{
		case AHEAD:
		moveCommand = STRAIGHT;
		break;
		
		case RIGHT:
		moveCommand = RIGHT_TURN;
		break;
		
		case LEFT:
		moveCommand = LEFT_TURN;
		break;
		
		case BACKWARD:
		moveCommand = U_TURN;
		break;
	}
	
	return moveCommand;
}
*/

void mazePrint(void)
{
	
	myprintf_P("\n\nBot Position X:%i  Y:%i\n", bot_position.x, bot_position.y);
	myprintf_P("Distance Counter: %d\n", distance_counter);
	uart_puts_P("Bot Orientation: ");
	if		(bot_orientation == NORTH) 
	uart_puts_P("NORTH\n");
	else if (bot_orientation == SOUTH) 
	uart_puts_P("SOUTH\n");
	else if (bot_orientation == WEST) 
	uart_puts_P("WEST\n");
	else if (bot_orientation == EAST) 
	uart_puts_P("EAST\n");
	
	// Drucke oberen Frame
	for (uint8_t x=0; x<MAZE_X; x++)
		uart_puts_P("+---");
	uart_puts_P("+\n");
	
	for(int8_t y=MAZE_Y-1; y>=0; y--)
	{
		
		uart_putc('|');
		// Drucke Zeilen
		for (uint8_t x=0; x<MAZE_X; x++)
		{
			
			if (x==bot_position.x && y==bot_position.y)
			{
				printf("%c%2d",'B' ,maze[x][y].distance);
			}
			else if(maze[x][y].dead) myprintf_P(" X ");
			else if(maze[x][y].start) myprintf_P("S%2i",maze[x][y].distance); 
			else if(maze[x][y].goal) myprintf_P("G%2i",maze[x][y].distance);
			else	printf("%3i", maze[x][y].distance);
			
			
			// Setzte Vertikale Wände
			if (x < MAZE_X-1)
			{
				if (maze[x][y].wall_E || maze[x+1][y].wall_W) uart_putc('|');
				else uart_putc(' ');
			}
			else uart_putc('|');

			
			
			//if (maze[x][y].goal) uart_putc('G');
			//if (maze[x][y].start) uart_putc('S');
			//uart_puts(itoa(maze[x][y].distance, buffer, 10));
		}
		uart_putc('\n');
		
		// Setze Horizontale Wände
		for (uint8_t x=0; x<MAZE_X; x++)
		{
			if (y > 0)
			{
				if(maze[x][y].wall_S || maze[x][y-1].wall_N) uart_puts_P("+---");
				else uart_puts_P("+   ");
			}
			else
			{
				if(maze[x][y].wall_S) uart_puts_P("+---");
				else uart_puts_P("+   ");
			}
			
		}
		uart_puts_P("+\n");
	}
	
}


void cellUpdate(intersection_t actual_intersection)
{
	if(actual_intersection == I_END)
		maze[bot_position.x][bot_position.y].goal = true;
	
	// Update actual Maze Cell
	if (!maze[bot_position.x][bot_position.y].visited)
		store_actual_walls(actual_intersection);
	
	maze[bot_position.x][bot_position.y].visited = true;
	maze[bot_position.x][bot_position.y].distance = distance_counter;
}


uint8_t maze_search(intersection_t actual_intersection)
{
	uint8_t nextMove;
	position_t nextCell;
	//direction_t nextDirection;
	bool stateFinished = false;
	static bool endFound = false;
	// Speicher ob noch unbesuchte Zellen sich vorm Bot befinden
	bool foundCell = false;
	
	while(!stateFinished)
	{
		switch(mazeState)
		{			
			/*********************************************************/
			case MAZE_SEARCH:
			
			// Aktuelle Position als besucht markieren
			maze[bot_position.x][bot_position.y].visited = true;
			
			// Ist das Ziel erreicht?
			if (maze[bot_position.x][bot_position.y].goal)
			{
				stateFinished = false;
				mazeState = MAZE_GOAL;
			}
			else
			{
				
				// Prüfe ob die nächsten Zellen ERREICHBAR und noch NICHT BESUCHT sind
				if ( cellPassable(orentationToDirection(AHEAD)) && !cellVisited(orentationToDirection(AHEAD)) )
				{
					nextCell = go_orientation(AHEAD);
					foundCell = true;
				}
				else if ( cellPassable(orentationToDirection(RIGHT)) && !cellVisited(orentationToDirection(RIGHT)) )
				{
					nextCell = go_orientation(RIGHT);
					foundCell = true;
				}	
				else if ( cellPassable(orentationToDirection(LEFT)) && !cellVisited(orentationToDirection(LEFT)) )
				{
					nextCell = go_orientation(LEFT);
					foundCell = true;
				}
				else
				{
					nextCell = bot_position;
					foundCell = false;
				}
	
				// Prüfung erfolgreich.
				if (foundCell)
				{
					mazeState = MAZE_SEARCH;
					stateFinished = true;
					distance_counter++;
					push(searchStack, bot_position);	// Speichere aktuelle Position auf Stack
					
					/*
						Aktualisiere botPosition und botOrientation mit neuen Werten
						Erzeuge Steuerkommando um zur nächsten Zelle zu kommen. 
					*/
					nextMove = go_Cell(nextCell);		
				}
				else
				{
					mazeState = MAZE_DEAD_END;
					stateFinished = false;
				}
			}
			break;
			
			/*********************************************************/
			case MAZE_DEAD_END:
			
			/* 
				Markiere Zelle als Sackgasse und fahre zur letzten Zelle
				Hole diese Zellkoordinaten vom Suchstack
				Gehe in den Rückfahrmodus (MAZE_BACKWARD)
			*/
			maze[bot_position.x][bot_position.y].dead = true;
			nextCell = pop(searchStack);
			distance_counter--;
			
			mazeState = MAZE_BACKWARD;
			stateFinished = true;
			nextMove = go_Cell(nextCell);
			
			break;
			
			/*************************************************************************
			* Fahre zurück über die Wegpunkte der im Stack							 *
			* gespeicherten Koordinaten, bis eine neue unbesuchte Zelle auftaucht.   *
			**************************************************************************/
			case MAZE_BACKWARD:
			
			// Bot ist wieder zum Startpunkt zurück gekehrt?
			// Suche beendet
			if (maze[bot_position.x][bot_position.y].start)
			{
				mazeState = MAZE_RESTART;
				stateFinished = true;
				bot_orientation = NORTH;
				nextMove = FINISH;
				break;
			}
			else
			{
				// Prüfe beim Zurückfahren ob die nächsten Zellen erreichbar UND noch nicht besucht sind
				if (	(cellPassable(orentationToDirection(AHEAD)) && !cellVisited(orentationToDirection(AHEAD))) ||
						(cellPassable(orentationToDirection(RIGHT)) && !cellVisited(orentationToDirection(RIGHT))) ||
						(cellPassable(orentationToDirection(LEFT)) &&  !cellVisited(orentationToDirection(LEFT)))      )
				{
					// Unbesuchte Zelle gefunden
					mazeState = MAZE_SEARCH;
					// Wurde das Ziel bereits erreicht suche unbekannten Weg nach kürzerem Weg zum Ziel
					if (endFound)
					{
						goalBranch = true;
						goalBranch_Position = distance_counter;
					}
				}
				else
				{
					if (!endFound)
					{		
						maze[bot_position.x][bot_position.y].dead = true;
						nextCell = pop(searchStack);
						distance_counter--;
						mazeState = MAZE_BACKWARD;
						stateFinished = true;
						nextMove = go_Cell(nextCell);
					}
					else
					{
						if (goalBranch && distance_counter > goalBranch_Position)
						{
							maze[bot_position.x][bot_position.y].dead = true;
						}
						else goalBranch = false;
						
						nextCell = pop(searchStack);
						distance_counter--;
						mazeState = MAZE_BACKWARD;
						stateFinished = true;
						nextMove = go_Cell(nextCell);
					}
				}
			}
			break;
			
			/************************************************************************/
			/* Goal reached                                                         */
			/************************************************************************/
			
			case MAZE_GOAL:
			
			endFound = true;
			
			//if (	(maze[bot_position.x][bot_position.y+1].distance < distance_counter - 1) && 
					//cellPassable(NORTH) && cellVisited(NORTH) ) 
			//{
				//goalBranch = true;
			//}
			//else if (	(maze[bot_position.x][bot_position.y-1].distance < distance_counter - 1) &&
					//cellPassable(SOUTH) && cellVisited(SOUTH) )
			//{
				//goalBranch = true;
			//}
			//else if (	(maze[bot_position.x+1][bot_position.y].distance < distance_counter - 1) &&
						//cellPassable(EAST) && cellVisited(EAST) )
			//{
				//goalBranch = true;
			//}
			//else if (	(maze[bot_position.x-1][bot_position.y].distance < distance_counter - 1) &&
						//cellPassable(WEST) && cellVisited(WEST) )
			//{
				//goalBranch = true;
			//}
			//else
			goalBranch = false;
			mazeState = MAZE_BACKWARD;
			stateFinished = false;
			
			break;
			
			/************************************************************************/
			/* Start re entered                                                     */
			/************************************************************************/
			
			case MAZE_RESTART:
			
			nextMove = FINISH;
			stateFinished = true;
			
			break;
			
			}
		}
		
	return nextMove;
}

uint8_t mazeRun(intersection_t actual_intersection)
{
	position_t newPosition;
	uint8_t newDirection = DIR_ERROR;
	
	distance_counter++;
	
	if (maze[bot_position.x][bot_position.y].start)
	{
		newPosition.x = bot_position.x;
		newPosition.y = bot_position.y+1;
		
		newDirection = go_Cell(newPosition);
		
		return newDirection;
	}
	
	if (maze[bot_position.x][bot_position.y].goal)
	{
		return FINISH;
	}
	
	if ((maze[bot_position.x][bot_position.y+1].distance == distance_counter) && 
		!maze[bot_position.x][bot_position.y+1].dead)
	{
		newPosition.x = bot_position.x;
		newPosition.y = bot_position.y+1;
		newDirection = go_Cell(newPosition);
	}
	else if ((maze[bot_position.x][bot_position.y-1].distance == distance_counter) && 
			!maze[bot_position.x][bot_position.y-1].dead)
	{
		newPosition.x = bot_position.x;
		newPosition.y = bot_position.y-1;
		newDirection = go_Cell(newPosition);
	}
	else if ((maze[bot_position.x+1][bot_position.y].distance == distance_counter) && 
			!maze[bot_position.x+1][bot_position.y].dead)
	{
		newPosition.x = bot_position.x+1;
		newPosition.y = bot_position.y;
		newDirection = go_Cell(newPosition);
	}
	else if ((maze[bot_position.x-1][bot_position.y].distance == distance_counter) && 
			!maze[bot_position.x-1][bot_position.y].dead)
	{
		newPosition.x = bot_position.x-1;
		newPosition.y = bot_position.y;
		newDirection = go_Cell(newPosition);
	}
	
	bot_position.x = newPosition.x;
	bot_position.y = newPosition.y;
	
	return newDirection;
}


