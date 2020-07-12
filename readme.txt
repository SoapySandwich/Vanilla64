Copyright (c) 2015 Magnus Auvinen


This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.


Please visit http://www.teeworlds.com for up-to-date information about 
the game, including new versions, custom maps and much more.

-------------------------------------------------------------------------

== Player information ==

This mod introduces some new concepts for the ball mod, which are described below.

;Grenade
: This is the ball (or Shotgun and Laser, depending on the variable '''sv_balltype'''). You can catch the ball. If you have a ball, you can't use any other weapon. The shotgun ammo reflects the time in seconds you can keep the ball before automatic shooting.
;Ninja
: This weapon is new for the ball mod. It replaces the old '/goalkeeper' chat command and provides a faster way to get a goalkeeper. On special maps, multiple goalkeepers are possible now.
;Goalkeeper
: The goalkeeper has multiple weapons, including the ninja mode. Hitting another player with the ninja will give you the ball. Goalkeeper do not have the jump limitation.
;Health
: By default the health is regenerating. Normal weapon hits will reduce your health. If your health reaches 0, you are stunned for some time.
;Armor
: On some maps you can pickup armor. Armor does not work against normal weapon attacks. Instead it reflects the bonus time to keep the ball for a longer time.
;Hammer
: As usual, you can get the ball from another player by using the hammer.
;Other weapons
: All other weapons make normal damage to tees and can stun them.

== Server settings ==

Some settings are in number of ticks. 1 second = 50 Ticks.

To avoid a disappearing ball on client side, you should set the shotgun_lifetime appropriate:
 tune shotgun_lifetime 1000000
 tune shotgun_curvature 10
 tune shotgun_speed 700

{|
! Option
! Default
! Value Range
! Description
|-
|sv_ball_multi
|0
|0-1
|With Multiball support enabled, the shotgun respawns regularly. Every player can keep a maximum of one ball. All other balls disappear.
|-
|sv_ball_goal_respawn
|1
|0-1
|Respawn all players after scoring.
|-
|sv_ball_respawn
|300
|0-100000
|Ball respawn time after scoring in ticks.
|-
|sv_ball_decay
|10
|0-100
|Ball bounce speed decaying. With a high value, the ball slows down faster.
|-
|sv_health_regen
|75
|0-10000
|Ticks between regenerating health.
|-
|sv_health_stunned
|200
|0-10000
|Number of ticks you are stunned when health is down to 0
|-
|sv_ball_base_keep
|150
|0-1000000
|Number of ticks you can keep the ball before automatic shooting.
|-
|sv_ball_armor_keep
|25
|0-1000000
|Number of ticks that are added to the ball keep time per armor you have.
|-
|sv_ball_lifetime
|90
|0-1000000
|Ball lifetime in seconds when no player touches the ball
|-
|sv_physical_start_vel
|100
|0-100
|How much influence does the tee velocity have on the projectile velocity. 100 = physically correct. 0 = no influence (vanilla teeworlds).
|-
|sv_balltype
|3
|2-4
|Ball Type (default 3:grenade; others, 2:shotgun; 4:laser)
|-
|sv_entergoal
|0
|0-1
|Whether permit player enter goal area or not (default 0), and you may need change this variable when changing maps. for example, when you use volleyball, you must set this variable to 1
|}

== Ball map development ==

The new entities are nearly equivalent to the new ones. You just have to change some things, like replacing the grenade-pickup through the shotgun and replace goalkeeper spawns. In the following I will describe all entities found in entities.png.

* Kill players of red team.
* Kill players of blue team.
* Goal of red team.
* Goal of blue team.
* Border for goalkeeper of red team.
* Border for goalkeeper of blue team.
* Goal of red team which kills players. This is necessary to enable slam dunks.
* Goal of blue team which kills players. This is necessary to enable slam dunks.
* This tile is solid only for balls, so they bounce at this tile.
* Exactly like the above tile, but additionally kills players.
* Kills no goalkeeper but all other players

At the bottom of the entities, in the pickup area:
* Shotgun = ball.
* Right of laser rifle is the spawn for red and blue goalkeeper. Don't use the empty ninja spawn.
