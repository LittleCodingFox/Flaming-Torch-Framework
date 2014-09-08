StartTime = 0

---
-- This is our game pre-initialize function.
-- It lets us start up any subsystems we need.
-- Some are essential for the program to even run, so we keep those around.
-- 
-- As a general rule, depending on what you want to do,
-- if a subsystem sounds important for it, register it for usage.
GamePreInitialize = function()
	-- We only want the log and clock for now.

	g_Log:Register()
	g_Clock:Register()
	
	-- We can toggle whether we want a Development Build Mode enabled or not.
	Game.DevelopmentBuild = true

	return true
end

---
-- This is where we actually do our game's startup.
-- If this returns false, the program stops immediately.
GameInitialize = function(Arguments)
	g_Log:Info("Closing in 5 seconds!")
	StartTime = g_Clock.Time()

	return true
end

---
-- This is our Frame Begin function, where we set up the frame to be drawn on screen.
-- Since we didn't create a renderer, it won't be called, ever.
GameFrameBegin = function(Renderer)
end

---
-- This is our Frame Draw function, where we draw most of the content of the frame.
-- Since we didn't create a renderer, it won't be called, ever.
GameFrameDraw = function(Renderer)
end

---
-- This is our Frame End function, where we draw the finishing touches to our frame,
-- such as a custom UI or an overlay.
-- Since we didn't create a renderer, it won't be called, ever.
GameFrameEnd = function(Renderer)
end

---
-- This is our Game Resize function, which is called whenever the window resizes.
-- Since we didn't create a renderer, it won't be called, ever.
GameResize = function(Renderer, Width, Height)
end

---
-- This is our Game Resources Reloaded function, which is called whenever our resource manager has reloaded our resources for us.
-- We may need some additional steps after that, which is why this function exists.
-- Since we didn't create a renderer, it won't be called, ever.
GameResourcesReloaded = function(Renderer)
end

---
-- This is our Frame Update function, where we update the game state each frame.
-- We may use GameFixedUpdate instead for fixed-step updates, however.
GameFrameUpdate = function()
end

---
-- This is our Fixed Update function, where we update the game state each time a Fixed Step frame happens.
-- We may use GameFrameUpdate instead for frame updates, however.
GameFixedUpdate = function()
end

---
-- This is our shutdown function.
-- We can clean up here before the program ends.
GameDeInitialize = function()
	return true
end

---
-- This is our Game Should Quit functin, which tells the program when the main loop is over
GameShouldQuit = function()
	return g_Clock.Difference(StartTime) >= 5000
end
