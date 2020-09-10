require('table')
local path = "..\\Assets\\Scripts\\"
dofile(path .. "Object.lua")
dofile(path .. "Character.lua")

Map = {}                                    
Map.mt = { __index = Map}

function Map:Create()
    local obj = 
    {
        m_width = 10,
        m_height = 10,
        m_maxDoorNum = 4,
        m_showDoors = true,
        m_description = {},
        m_doors = {},
        m_objs = {},
        m_tiles = {} 
    }
    self.m_player = Player(3, 2)
    obj.map = obj or {}
    setmetatable(obj, Map.mt)
    return obj
end

Map.call = { __call = Map.Create }
setmetatable(Map, Map.call)

function Map:Update() 
  print("--- Update --- ")
  self:Render();
  self:GetInput();
end

function Map:Render(player)
    for y = 1, self.m_height, 1 do
        for x = 1, self.m_width, 1 do
            if(x == player:GetX() and y == player:GetY()) then
                player:Render()
            elseif(x == 1 or x == self.m_width or y == 1 or y == self.m_height) then
                io.write('* ')
            elseif(self:CheckDoor(x, y) and self.m_showDoors == true) then
                io.write('D ')
            else
                self:RenderOthers(x, y)
            end
        end
        io.write('\n')
    end
    
    print("=====================================================")
    print("Player X: " .. player:GetX())
    print("Player Y: " .. player:GetY())
    print("=====================================================")
    print("[ Description ]")
    for key, value in pairs(self.m_description) do
        print(value)
    end
    print("=====================================================")

end

function Map:RenderOthers(x, y)
    if(self:RenderObjects(x, y) == true) then
        return
    elseif(self:RenderWaterTiles(x, y) == true) then
        return
    end
    
    io.write('  ')
end

function Map:RenderObjects(x, y)
    local count = self:GetObjCount()

    for i = 1, count, 1 do
        if(x == self.m_objs[i]:GetX() and y == self.m_objs[i]:GetY()) then
            self.m_objs[i]:Render()
            return true
        end
    end

    return false
end

function Map:AddDescription(str)
    table.insert(self.m_description, str)
end

function Map:GetWidth()
    return self.m_height
end

function Map:GetHeight()
    return self.m_width
end

function Map:GetMapSize()
    return self.m_height
end

function Map:SetDoorShow(value)
    self.m_showDoors = value
end

function Map:DoorShown()
    return self.m_showDoors
end

function Map:CheckDoor(x, y)
    local count = self:GetDoorCount()

    for i = 1, count, 1 do
        if(x == self.m_doors[i]:GetX() and y == self.m_doors[i]:GetY()) then
            return true
        end
    end

    return false
end

function Map:AddObjects(obj)
    table.insert(self.m_objs, obj)
end

function Map:GetDoor(x, y)
    local count = self:GetDoorCount()

    for i = 1, count, 1 do
        if(x == self.m_doors[i]:GetX() and y == self.m_doors[i]:GetY()) then
            return self.m_doors[i]
        end
    end

    return nil
end

function Map:AddWaterTiles(tiles)
    local tile = nil

    for key, value in pairs(tiles) do
        -- print(value[1])
        -- print(value[2])
        tile = WaterTile(value[1], value[2])
        table.insert(self.m_tiles, tile)
    end
end

function Map:AddDoor(obj)
    local count = self:GetDoorCount()

    if(count == self.m_maxDoorNum) then
        return
    end

    table.insert(self.m_doors, obj)
end

function Map:CheckBoundary(input, player)
    local playerX = player:GetX()
    local playerY = player:GetY()

    if(input == 'w') then
        if((playerY <= 2) or self:CheckWaterTiles(playerX, playerY - 1, player:CanSwim())) then
            return true
        end
    elseif(input == 'a') then
        if((playerX <= 2) or self:CheckWaterTiles(playerX - 1, playerY, player:CanSwim())) then
            return true
        end
    elseif(input == 's') then
        if((playerY >= self.m_height - 1) or self:CheckWaterTiles(playerX, playerY + 1, player:CanSwim())) then
            return true
        end
    elseif(input == 'd') then
        if((playerX >= self.m_width - 1) or self:CheckWaterTiles(playerX + 1, playerY, player:CanSwim())) then
            return true
        end
    end

    return false
end

function Map:CheckWaterTiles(x, y, canSwim)
    if(self.m_tiles == nil)then
        return false
    end

    if(canSwim == true) then
        return false
    end

    for i = 1, #self.m_tiles, 1 do
        if(x == self.m_tiles[i]:GetX() and y == self.m_tiles[i]:GetY()) then
            return true
        end
    end

    return false
end

function Map:RenderWaterTiles(x, y)
    if(self.m_tiles == nil) then
        return false
    end

    -- for i = 1, #self.m_tiles, 1 do
    --     print(self.m_tiles[i]:GetX())
    --     print(self.m_tiles[i]:GetY())
    -- end

    
    --local count = self:GetTileCount()

    for i = 1, #self.m_tiles, 1 do
        if(x == self.m_tiles[i]:GetX() and y == self.m_tiles[i]:GetY()) then
            self.m_tiles[i]:Render()
            return true
        end
    end

    return false
end

function Map:CheckCollision(player, input)
    local count = self:GetObjCount()
    
    if(input == "pick") then
        for i = 1, count, 1 do
            if(player:GetX() == self.m_objs[i]:GetX() and player:GetY() == self.m_objs[i]:GetY()) then
                if(self.m_objs[i]:GetTag() == "Key") then
                    player:RootEscapeKey(true)
                    table.remove(self.m_objs, i)
                    self:SetDoorShow(true)
                elseif(self.m_objs[i]:GetTag() == "TreasureKey") then
                    player:RootTreasureKey(true)
                    table.remove(self.m_objs, i)
                elseif(self.m_objs[i]:GetTag() == "SwimmingSuit") then
                    player:RootFin(true)
                    table.remove(self.m_objs, i)
                end
            end
        end
    elseif(input == "unlock") then
        for i = 1, count, 1 do
            if(player:GetX() == self.m_objs[i]:GetX() and player:GetY() == self.m_objs[i]:GetY()) then
                if(self.m_objs[i]:GetTag() == "Treasure" and player:HasTreasureKey()) then
                    player:RootTreasure(true)
                end
            end
        end
    end
end

function Map:GetDoorCount()
    -- local count = 0
    -- for key, value in pairs(self.m_doors) do
    --     count = count + 1
    -- end

    return #self.m_doors
end

function Map:GetObjCount()
    -- local count = 0
    -- for key, value in pairs(self.m_objs) do
    --     count = count + 1
    -- end

    return #self.m_objs
end

function Map:GetTileCount()
    return #self.m_tiles
end


WaterTile = {}                                    
WaterTile.mt = { __index = WaterTile}

function WaterTile:Create(x, y)
    local obj = 
    {
        m_x = x,
        m_y = y
    }
    obj.map = obj or {}
    setmetatable(obj, WaterTile.mt)
    return obj
end

WaterTile.call = { __call = WaterTile.Create }
setmetatable(WaterTile, WaterTile.call)

function WaterTile:GetX()
    return self.m_x
end

function WaterTile:GetY()
    return self.m_y
end

function WaterTile:Render()
    io.write('W ')
end