water = gr.material({0.2, 0.2, 0.2}, {0.9, 0.9, 0.9}, 25)
mirror = gr.material({0.6975, 0.6375, 0.17025}, {0.9, 0.9, 0.9}, 25)
mirror2 = gr.material({0, 0.6375, 0.17025}, {0.9, 0.9, 0.9}, 25)
black = gr.material({0,0,0}, {0.1,0.1,0.1}, 25)

scene_root = gr.node('root')
scene_root:rotate('X', 23)
scene_root:translate(6, -2, -15)

lake = gr.mesh('lake', 'Assets/plane.obj')
scene_root:add_child(lake)
lake:set_material(water)
lake:scale(20, 20, 6.1)


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

ball = gr.nh_sphere('ball', {-2, 3, -2}, 3)
scene_root:add_child(ball)
ball:set_material(mirror)

cube = gr.nh_box('cube', {-9, 0, 2}, 3)
scene_root:add_child(cube)
cube:set_material(mirror2)
cube:rotate('Y', -20)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene_root, 'sample.png', 500, 500, 
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {white_light})
