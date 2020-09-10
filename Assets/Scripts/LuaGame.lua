local path = "..\\Assets\\Scripts\\"
dofile(path .. "Map.lua")
dofile(path .. "Object.lua")
dofile(path .. "Player.lua")

LuaGame = {}                                    
LuaGame.mt = { __index = LuaGame}

function LuaGame:Create(obj)
    local obj = {
        m_currentMap = nil, 
        m_maps = {}
        --m_map = Map()
    }
    self.m_loop = true
    self.m_treasureOpend = false
    self.m_player = Player(3, 2)

    obj.game = obj or {}
    setmetatable(obj, LuaGame.mt)
    return obj
end

LuaGame.call = { __call = LuaGame.Create }
setmetatable(LuaGame, LuaGame.call)

function LuaGame:Init()

    --[[ Map
                    [4]
                     |
        [1] - [2] - [3] - [5]
                     |
                    [6]
    ]]

    --[[
        north: (5, 2, 5, 8)
        south: (5, 9, 5, 3)
        right: (9, 5, 3, 5)
        left : (2, 5, 8, 5)
    ]]
    table.insert(self.m_maps, Map())
    table.insert(self.m_maps, Map())
    table.insert(self.m_maps, Map())
    table.insert(self.m_maps, Map())
    table.insert(self.m_maps, Map())
    table.insert(self.m_maps, Map())

    -- Map 1
    local door = Door(9, 5, 3, 5)
    door:SetConnectedMap(self.m_maps[2])
    self.m_maps[1]:AddDoor(door)

    self.m_maps[1]:AddDescription("Welcome to the simple maze game. Please read this description before you get into.")
    self.m_maps[1]:AddDescription("There are six rooms you have to explore.")
    self.m_maps[1]:AddDescription("Your goal is looking for the key to open the treasrue chest which is somewhere in the game.")
    self.m_maps[1]:AddDescription("Player character is 'P', type [w, a, s, d] and [enter] to move your character.")
    self.m_maps[1]:AddDescription("You can force-close the game by typing [q]' and enter")

    -- Map 2  
    door = Door(2, 5, 8, 5) -- Left
    door:SetConnectedMap(self.m_maps[1])
    self.m_maps[2]:AddDoor(door)
    
    door = Door(9, 5, 3, 5) -- Right
    door:SetConnectedMap(self.m_maps[3])
    self.m_maps[2]:AddDoor(door)
    self.m_maps[2]:SetDoorShow(false)

    self.m_maps[2]:AddDescription("You've found the way to the another room.")
    self.m_maps[2]:AddDescription("Doors are represented as 'D', but some of them aren't visible.")
    self.m_maps[2]:AddDescription("In that case, you have to find the way to make them visible.")
    self.m_maps[2]:AddDescription("Ah, there is a key, Why don't you [pick] it up?")

    local key = Key(5, 2)
    self.m_maps[2]:AddObjects(key)

    -- Map 3
    door = Door(5, 2, 5, 8)  -- North
    door:SetConnectedMap(self.m_maps[4])
    self.m_maps[3]:AddDoor(door)

    door = Door(5, 9, 5, 3)  -- South
    door:SetConnectedMap(self.m_maps[6])
    self.m_maps[3]:AddDoor(door)

    door = Door(9, 5, 3, 5)  -- Right
    door:SetConnectedMap(self.m_maps[5])
    self.m_maps[3]:AddDoor(door)

    door = Door(2, 5, 8, 5)  -- Left
    door:SetConnectedMap(self.m_maps[2])
    self.m_maps[3]:AddDoor(door)

    self.m_maps[3]:AddDescription("Great work! You've found the key to make doors visible")
    self.m_maps[3]:AddDescription("Three doors are here. Where would you like to go?")

    -- Map 4
    door = Door(5, 9, 5, 3)  -- South
    door:SetConnectedMap(self.m_maps[3])
    self.m_maps[4]:AddDoor(door)

    local waterTiles = 
    {
        {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4},
        {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5}, {7, 5}, {8, 5}, {9, 5},
        {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6}, {7, 6}, {8, 6}, {9, 6}        
    }
    self.m_maps[4]:AddWaterTiles(waterTiles)

    local treasureKey = Key(5, 2)
    treasureKey:SetTag("TreasureKey")
    self.m_maps[4]:AddObjects(treasureKey)

    self.m_maps[4]:AddDescription("Hmmm... Hmm ... There's a key, but a river blocking it in the middle.")
    self.m_maps[4]:AddDescription("It looks too deep to [swim] without any gear.")
    self.m_maps[4]:AddDescription("Let's find the item to cross the river")

    -- Map 5
    door = Door(2, 5, 8, 5)  -- Left
    door:SetConnectedMap(self.m_maps[3])
    
    local treasure = Treasure(7, 5)
    self.m_maps[5]:AddObjects(treasure)

    
    self.m_maps[5]:AddDoor(door)
    self.m_maps[5]:AddDescription("Look! The treasure chest!")
    self.m_maps[5]:AddDescription("But, it's locked. Let's use the key to [unlock] this")


    -- Map 6
    door = Door(5, 2, 5, 8)  -- North
    door:SetConnectedMap(self.m_maps[3])
    self.m_maps[6]:AddDoor(door)
    
    local swimmingSuit = SwimmingSuit(5, 9)
    self.m_maps[6]:AddObjects(swimmingSuit)

    self.m_maps[6]:AddDescription("Look! Something over there")
    self.m_maps[6]:AddDescription("It's a swimming suit!")
    
    self.m_currentMap = self.m_maps[1]
end

function LuaGame:Update() 
    while (self.m_loop == true and self.m_treasureOpend ~= true) do
        os.execute("cls")
        self:Render()
        self:GetInput()
        self:LateUpdate()
    end
    
    if(self.m_treasureOpend == true) then
        os.execute("cls")
        print("Congraturation! You've claimed the treasure!")
        print("Thank you for plyaing this game")
    end
end

function LuaGame:LateUpdate()
    if(self.m_currentMap == nil) then
        return
    end

    if (self.m_currentMap:CheckDoor(self.m_player:GetX(), self.m_player:GetY()) and self.m_currentMap:DoorShown()) then
        local door = self.m_currentMap:GetDoor(self.m_player:GetX(), self.m_player:GetY())

        if(door == nil) then
        end
        
        local connectedMap = door:GetConnectedMap()
        self.m_currentMap = connectedMap
        self.m_player:SetPosition(door:GetSpawnX(), door:GetSpawnY())
    end

    self.m_treasureOpend = self.m_player:CheckRootTreasure()
end

function LuaGame:Render()
    self.m_currentMap:Render(self.m_player)
end

function LuaGame:TreasureOpend()
    return self.m_treasureOpend
end

function LuaGame:GetInput()
    print("What would you like to do?")
    io.write("Input: ")
    local input = io.read("*line")
    if(input == 'q') then
        self.m_loop = false
        return
    end

    --self.m_currentMap:
    self.m_currentMap:CheckCollision(self.m_player, input)

    -- Player character ---
    if(self.m_player ~= nil) then
        if(self.m_currentMap:CheckBoundary(input, self.m_player) == false) then
            self.m_player:HandleInput(input)
        end
    end
end