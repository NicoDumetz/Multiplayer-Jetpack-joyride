/*
** EPITECH PROJECT, 2025
** B-NWP-400-LIL-4-1-jetpack-nicolas.dumetz
** File description:
** settings
*/

#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

/******************************************************************************/
/*                                                                            */
/*                                PROTOCOL                                    */
/*                                                                            */
/******************************************************************************/

#define LOGIN_REQUEST           0x01 // Sent by the client to request a login
#define PLAYER_ACTION           0x05 // Sent by the client to indicate a player action (e.g., jump)
#define LOGIN_RESPONSE          0x02 // Sent by the server to confirm login and assign a player ID
#define MAP_TRANSFER            0x03 // Server sends the entire map to the client
#define GAME_START              0x04 // Game begins (all players are ready)
#define GAME_STATE              0x06 // Server sends current state of each player (alive/dead)
#define POSITION_UPDATE         0x07 // Server sends updated position (x, y) of a player
#define COIN_EVENT              0x08 // Player has collected a coin
#define PLAYER_ELIMINATED       0x09 // Player has been eliminated (e.g., by zapper)
#define ACTION_ACK              0x0A // Server acknowledges the reception of a valid action
#define WAITING_PLAYERS_COUNT   0x0B // Server tells how many players are connected and waiting
#define GAME_OVER               0x10 // Server notifies that the game is over

/******************************************************************************/
/*                                                                            */
/*                             GAME SIMULATION                                */
/*                                                                            */
/******************************************************************************/

#define NUMBER_CLIENTS          2       // Number of players required to start the game
#define TICK_RATE               60.0f   // Number of simulation ticks per second
#define SCROLL_SPEED            2.0f    // Horizontal scroll speed in tiles per second
#define GRAVITY_SPEED           6.0f    // Vertical falling speed (gravity), tiles per second
#define JETPACK_JUMP_SPEED      6.0f    // Jump boost when pressing space, tiles per second
#define TICK_INTERVAL           (1.0f / TICK_RATE) // Duration of a single simulation tick
#define JUMP_DELTA_Y            1.0f    // Jump delta Y (not always used)
#define COLLISION_MARGIN_LEFT    0.50f
#define COLLISION_MARGIN_RIGHT   0.30f
#define COLLISION_MARGIN_TOP     0.01f
#define COLLISION_MARGIN_BOTTOM  0.10f

/******************************************************************************/
/*                                                                            */
/*                                DISPLAY                                     */
/*                                                                            */
/******************************************************************************/

#define WINDOW_WIDTH            800     // Width of the game window (pixels)
#define WINDOW_HEIGHT           600     // Height of the game window (pixels)
#define TILE_ROWS               10      // Number of vertical tiles in the grid
#define TILE_MARGIN             50      // Margin (top and bottom) around the grid
#define TILE_SIZE               ((WINDOW_HEIGHT - TILE_MARGIN * 2) / TILE_ROWS) // Size of one tile (square)

/******************************************************************************/
/*                                                                            */
/*                                 PLAYER                                     */
/*                                                                            */
/******************************************************************************/

#define SPRITE_SHEET_WIDTH      538     // Total width of the sprite sheet
#define SPRITE_SHEET_HEIGHT     803     // Total height of the sprite sheet

#define PLAYER_SCALE            0.5f    // Scaling factor for player rendering
#define PLAYER_ORIGIN_X         0.5f    // Horizontal origin of the sprite (center)
#define PLAYER_ORIGIN_Y         0.5f    // Vertical origin of the sprite (center)

#define PLAYER_SPRITE_WIDTH     (SPRITE_SHEET_WIDTH / 4) // One frame width (4 frames per row)
#define PLAYER_SPRITE_HEIGHT    (SPRITE_SHEET_HEIGHT / 6) // One frame height (6 rows total)

#define ANIMATION_FRAME_TIME    0.45f    // Time between frames in seconds
#define FIXED_PLAYER_X          (4 * TILE_SIZE) // X position where players are drawn

/******************************************************************************/
/*                                                                            */
/*                            NETWORK PACKETS                                 */
/*                                                                            */
/******************************************************************************/

#define INVALID_TILE_ID         255     // Value used to represent an invalid tile
#define INVALID_ID              255     // Value used to represent an invalid player ID

#define FRAGMENT_SIZE           1024U   // Max size of one packet fragment

#endif /* SETTINGS_HPP_ */
