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

// ──────────────── CLIENT → SERVER ────────────────

#define LOGIN_REQUEST           0x01  // Login request: sent by the client to initiate a connection (payload: empty or expected player count if defined)
#define PLAYER_ACTION           0x05  // Player action (payload: 1 byte, e.g., 0x01 = jump)

// ──────────────── SERVER → CLIENT ────────────────

#define LOGIN_RESPONSE          0x02  // Login response (payload: [player_id, 0x00, expected_player_count])
#define MAP_TRANSFER            0x03  // Full map transfer (payload: [player_id, map as textual layout])
#define GAME_START              0x04  // Signals the start of the game
#define GAME_STATE              0x06  // Current state of all players (payload: [id1, alive1, id2, alive2, ...])
#define POSITION_UPDATE         0x07  // Position update (payload: [player_id, float x, float y])
#define COIN_EVENT              0x08  // Coin collected (payload: [player_id, int x, int y]) → *optional depending on design*
#define PLAYER_ELIMINATED       0x09  // Notification that a player has been eliminated (payload: [player_id])
#define ACTION_ACK              0x0A  // Acknowledgment of a valid player action (payload: [0x05])
#define WAITING_PLAYERS_COUNT   0x0B  // Server informs how many players are connected and waiting (payload: [count])
#define GAME_OVER               0x10  // End of game notification (payload: [winner_id] or 255 if tie)

/******************************************************************************/
/*                                                                            */
/*                             GAME SIMULATION                                */
/*                                                                            */
/******************************************************************************/

#define NUMBER_CLIENTS_DEFAULT          2
#define NUMBER_CLIENTS_MAX              9

#define TICK_RATE               60.0f   // Number of simulation ticks per second
#define SCROLL_SPEED            2.0f    // Horizontal scroll speed in tiles per second
#define GRAVITY_SPEED           6.0f    // Vertical falling speed (gravity), tiles per second
#define JETPACK_JUMP_SPEED      6.0f    // Jump boost when pressing space, tiles per second
#define TICK_INTERVAL           (1.0f / TICK_RATE) // Duration of a single simulation tick
#define JUMP_DELTA_Y            1.0f    // Jump delta Y (not always used)
#define COLLISION_MARGIN_LEFT    0.50f
#define COLLISION_MARGIN_RIGHT   0.50f
#define COLLISION_MARGIN_TOP     0.01f
#define COLLISION_MARGIN_BOTTOM  0.20f

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
#define MAX_LINES              10      // Maximum number of Lines on the map

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

#define ANIMATION_FRAME_TIME    0.1f    // Time between frames in seconds
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
