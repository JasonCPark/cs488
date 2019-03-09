water = gr.material({0.2, 0.2, 0.6}, {0.9, 0.9, 0.9}, 25)
wallMat = gr.material({0.1, 0.5, 0.1}, {0.2, 0.2, 0.2}, 25)
mirror = gr.material({0.6975, 0.6375, 0.17025}, {0.9, 0.9, 0.9}, 25, 1.5, true)
mirror2 = gr.material({0, 0.6375, 0.17025}, {0.9, 0.9, 0.9}, 25)
black = gr.material({0,0,0}, {0.5,0.5,0.5}, 25)

scene_root = gr.node('root')
scene_root:rotate('X', 23)
scene_root:translate(6, -2, -15)

lake = gr.mesh('lake', 'Assets/planeCheckerboard.obj')
scene_root:add_child(lake)
lake:set_material(black)
lake:scale(30, 30, 30)
lake:translate(-5,0,-20)

wall = gr.mesh('wall', 'Assets/plane.obj')
--scene_root:add_child(wall)
wall:set_material(wallMat)
wall:scale(20, 20, 6.1)
wall:rotate('X', 90)
wall:translate(0,4,-15)

--[[
stagger = 4
for j = 1,3 do
	for i = 1,3 do
		tile = gr.mesh('tile', 'Assets/plane.obj')
		scene_root:add_child(tile)
		tile:set_material(black)
		tile:scale(2,2,2)
		tile:translate(-22 + i*8 + stagger,0.1,-7 + j*4)
	end
	if stagger == 4 then stagger = 0 else stagger = 4 end
end
]]

ball = gr.nh_sphere('ball', {-2, 3, -6}, 3)
scene_root:add_child(ball)
ball:set_material(mirror)

ball = gr.nh_sphere('ball', {-9, 3, -6}, 3)
scene_root:add_child(ball)
ball:set_material(mirror2)

cube = gr.nh_box('cube', {-2, 2.5, 3}, 3)
--scene_root:add_child(cube)
cube:set_material(mirror2)
cube:rotate('Y', -50)

tree = gr.mesh('tree', 'Assets/tree.obj')
--scene_root:add_child(tree)
tree:set_material(mirror)
tree:scale(0.4,0.4,0.4)
tree:translate(-9, 0, 2)

white_light = gr.light({-200.0, 100.0, 200.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
white_light2 = gr.light({200.0, 150.0, 200.0}, {0.4, 0.4, 0.4}, {1, 0, 0})
white_light3 = gr.light({0.0, 250.0, -100.0}, {0.3, 0.3, 0.3}, {1, 0, 0})

gr.render(scene_root, 'sample.png', 400, 400, 
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {white_light, white_light3})
