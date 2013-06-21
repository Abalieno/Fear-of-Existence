Fear-of-Existence
=================

[b]Ok, this is essentially like a joke since my game is in an purely conceptual stage, it will take me years to get closer to a playable/interesting stage, and I'm not asking for money, neither now nor later. (but I could always use some help)[/b]

Maybe someone can find the ideas involved in this project interesting and inspiring.

FoE is a project shell containing the 'ideal' for an ambitious roguelike. More than a faithful roguelike, it is intended as an hybrid, that will mix together many, many old-school games and styles, back from the days of the AMIGA. Not only, but it will borrow ideas, maybe cameos, from modern fantasy literature, anime and lots of other different material. A strong influence will come from Malazan (from the literature side) and Dark Souls (from the current games side).

There are plenty of complex and ambitious roguelikes out there, but I think none, beside Incursion in its own way, tried to recreate a full RPG system. For RPG system I intend a fully open ruleset like pen and paper RPGs. Pathfinder, Rolemaster, Dangerous Journey, Elric!, Harnmaster, to name a few of those that I'm interested in. So a roguelike that will mimic a real ruleset, without hiding its mechanics behind complex math, and that will be run entirely through 'open' mechanics and rules, with dice rolls and everything. Borrowing especially from those rulesets the detail and complexity. See my further considerations on why using a full ruleset: [b]http://www.cesspit.net/drupal/node/2067[/b]

The other important point is that I'm not a programmer, so the hope of even getting close to the goals are pretty dim. But this process is the part of the project, right now, that could make it interesting. I'm learning how to program from scratch, by working directly on this game concept. I learn a few things every day, make plenty of mistakes. Not only what I'm doing and will do is completely open source freely available at all stages, but I'll also keep a diary of my (sporadic) successes and (many) failures. So someone who's as clueless as me about game programming (but still interested about it) could tag along and see what I'm doing. It's not just open source (very bad) code, but code that will be explained through all its iterations step by step, so that someone as clueless as me could still understand it at least as much as I do, and maybe eventually make a better progress than me.

Those are the two main important points: the recreation of a full, complex RPG system and the ongoing open development.

Long-term, the game has quite an ambitious set of goals that will keep me busy for several centuries:

[list]
[*] Persistent, handcrafted world (including dungeons) 
[*] Overworld and travel (ADOM-like)
[*] Story and quests (ADOM-like)
[*] Systemic flow, with linear branches (a number of optional paths, or go freeform sandbox mode)
[*] No classes
[*] No levels
[*] Skill based (but not strictly use-based)
[*] Full turn based combat with movement points. Influences from Vagrant Story and others.
[*] Full party system (joinable persistent NPCs, up to four, including player)
[*] Realistic looting (no coins or armor dropping from rats)
[*] No infinitely respawning mobs
[*] Resource/consumables system (basic crafting, need to sleep, food, drink)
[*] Settlement mode and city management (very long term)
[*] Menu-driven (no memorization of esoteric commands required)
[/list]

The idea is to work toward a fully realized game-world that still has a 'story' to follow, but that it can also be 'lived-in' in a sort of sandbox-lite mode. Where it is the player who decides what to do and when. Including if to go after the story(ies) or completely ignore it.

All that stuff is moved far to the future, because first I need to have a shell of a game that can support all that. What I'm currently doing is learn C++ (on my own, thanks to some help online, and some books) and working with the libtcod library. Right now going through tutorials and implementing a shell of a combat system.

How far this project will actually go I cannot say. But I'm at least committed to it and if nothing changes I'll keep trying. Despite what I'll able to do or not, I believe there are plenty of absolutely great ideas involved, and that could make this project interesting for other people, maybe simply as inspiration. I doubt anyone wants to help, but if you do you're welcome ;)

Some other details on the system:
As I said, skill based and no levels/classes. Since the goal is a fully realized world, and not a modular, randomized dungeon like most RL, I can't use permdeath. I don't know how death will be handled, but a character will have access to all systems, without the need to reroll, so there's no real need for replayability and the focus will be instead on longevity (depth and breadth). The experience system will be similar to Dark Souls, where the points earned can be spent (and lost if something goes wrong) as a form of currency to improve attributes and statistics.

The party system instead will be 'virtualized' out of combat, so you'll move around just one character. But whenever combat is initiated (either automatically or manually) all party members will be 'deployed', and will then be moved independently in turn-based combat. Similarly to those lovely GoldBox games we all love (but with a much more complex and deeper system governing combat).

Being based on libtcod, and so a console-like, ASCII-only game, there won't be 'graphic' of any kind. But I plan to use a full unicode font, and so will creatively use all sort of weird symbols, expanding greatly the limit of the standard ASCII range and getting closer to a stylized tileset (on top of full colors and large game area, which the library supports).

Some more information.
On twitter: @MrSkimpole
My site: http://www.cesspit.net/ (but not much about this project yet)
A forum thread where I go in detail about what I do (and how I fail): [b]http://www.quartertothree.com/game-talk/showthread.php?t=70123[/b]
Another thread, at the library forum: http://doryen.eptalys.net/forum/index.php?topic=1500.0
And from time to time I'm on IRC asking for help: irc://irc.quakenet.org:6667/rgrd

P.S.
A possible temporary spin-off that can be done early with what I have is titled: WYSIATI

WYSIATI is an acronym for an interesting philosophical idea: What You See Is All There Is.

Limited to a game, the 'innovative' idea is to create a 'subjective' world violating some normal rules. The basis is: only what's currently in the player 'Field of View' exists. Not only what 'exits' the player's view stops existing, but it also has a chance or being 'transformed'. For example you see a room with a wall on your left? If you turn, and then turn back, maybe that wall will now have a door! This applies to eventual monsters too. You see a dragon right about to spit fire on you! You turn, and as long you're turned away the dragon stops existing, and so can't harm you. But if you turn back, maybe the dragon disappeared, maybe the dragon turned into a goblin or a chest, or maybe the dragon is still there, and in the meantime it got closer! You die.

I'm starting a very basic changelog to track a few things:

v0.0021 http://www.cesspit.net/misc/prog/foe021.zip
- Added mouse support (this will be used to quickly check what ASCII symbols stand for, by simply mouse-over)
- Switched to continuous game loops both in and out of combat, capped at 20 FPS
- FIX: fixed orc monster types resetting movement cap beyond maximum value

v0.0020 http://www.cesspit.net/misc/prog/foe020.zip
- Added dynamic initiative list on the right panel during the player's turn
- Added a colored bar to represent the player's health, along with movement points (this only shown in-combat)
- Re-added the monster 'trails' effect when they move
