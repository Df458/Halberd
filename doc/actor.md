Actor Stuff List
================

Attributes
----------
- Position
- Direction
- Sprites (Idle, move, others for script calling only. If missing, defaults to
  idle)

Settings
--------
- Lock to grid (4 direction, tile movements vs. 8 direction, free movements)
- Orients (can turn when moving/at all)
- Constrain movement with solids
- Visible(rendered)
- Solid(block other objects)

Script Types
------------
- Created
- Destroyed
- Collides(With other actor)
- Tick
- Operated(By another actor, when action key is pressed.)

Actor Util Functions
--------------------
- Create
- Destroy
- Turn(sets orientation)
- Move
- Path(manual)
- Path(auto)
- Wait
- Animate(sets sprite)
- Operate(operates object(s) in front)

General Util Functions
----------------------
- Text box
- Dialog tree
- Start battle
- Give/Take item
- Give/take money
- Sound
