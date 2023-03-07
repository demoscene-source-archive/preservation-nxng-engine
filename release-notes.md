# Release notes

[v2.0 - 9.0.0.6]
- All memory leaks where tracked down and killed! [:
- Added Sequence support (animated texture map). Two restrictions however! There must be 4 digits in the sequence filenames and the first clip must be numbered 0001! FPS is fixed to 20.
- Better automatic sizing,
- Better Surface decoder requiring less memory while allowing upto 4096 surfaces per entity (:,
- Corrected lotsa small bugs with GUI, implemented LOADER class system,
- Removed an ugly bug in the new BoundingBox system that was a potential cause to total crash with many many scenes,
- TAB toggles stats mode, F1 force use of Single-Texture rasterizer, F2 toggles free camera mode.

[v2.0 - 9.0.0.5]
- GUI is now using automatic scene sizing... This might cause problems with 16bit Zbuffer,
- Added alternate rasterizer for 3d cards that do not support multitexturing,
- Removed brute force resolving in Lightmap computing. Calculations now uses a wrecked BSP giving speed improvement ranging from 2x to 10x over brute force method,
- Lightmaps are now optimized and stored at minimal A.R. possible,
- Removed edge sharing artifacts in Lightmaps.

[v2.0 - 9.0.0.3]
- This is a WORK-IN-PROGRESS release,
- Radiosity version handles shadows correctly! Even alpha textures are ok... Kweeel... [: But this is incredibly fucking slow as hell that sucks!!! (got it? (:)
- Animated Light/Shadow maps,
- nX can now compute Shadow-only maps. Much lighter they can be pretty bigger and make still available realtime lightning engine.

[v2.0 - 9.0.0.1]
- Major cleanup in code, classes only now...
- Enhanced interface with a console output like Quake [;
- RAYTRACED Lightmaps, supporting Alpha-Texture, Transparency to cast shadows and phong shaded lightmaps. Radiosity version is also working but not available since it do not handle shadows properly ))): and alpha textures definitively are a big problem when using radiosity.
Lightmaps are perfectly UV-Mapped! (:

-=- CHANGES FROM nXs3 TO nXng -=-

- v1.7c:
	- Added the EXTENDED SURFACE RENDERSTATES section to this doc,
	- [ADDEVMAP] surface keyword added to allow COLORMAP + EVMAP to
		be blended using ADDITIVE instead of the default MULTIPLY,
	- Fixed an odd bug in envelops, should fix a lot of weird bugs (;
	- Missing objects no more crash the engine,
	- Missing filename is always given in error messages [:,
	- Relooked the GUI..

- v1.7b:
	- Better memory manager, greatly improved memory requirement!
	- Rewrote software sub pixel/texel corrections... gave a little speedup,
	- Boost in D3D (again!? yes again... [: but a minor one),
	- Improved distribution look (:,
	- Fixed a minor bug with POLS reader... hmm very minor (:
		nX replays 100% of Lightwave's samples!
	- Speedups in D3D,
	- Color conversions are ok. Now 32bpp to 15/16/24 works,
	- Fixed a crash with very and fucking old drivers reporting max texture size of 0!,
	- Reintroduced specular in d3d. Software soon, but i'm laaaaazy as usual (:,
	- Memory hole is past... [:
	- Colored fog in soft is back, fog limits supported (envellop/fixed)...
	- Added the [NOLIGHT] surface name keyword in case you're using the 'Shade ON'
		option,

- v1.7:
	- Z-buffer optimized on a per-frame basis,
	- Removed a silly bug in MOA... now MOA always works! (:
	- Greatly improved render state setup logic... old accelerators should
		work really better now!,
	- Woops! Preca focal envellop support was not up-to-date ): FIXED! (:,
	- Added Truemap + Additive to piXel! as an optimized call,
		(Quite slow anyway...)
	- Added Truemap + Gouraud to piXel! as an optimized call,
		(Quite slow anyway...)
	- Now using BASS Sound System! It kicks ASS!
	- Overall Speed-up,
	- Switched back to xAudio for a small amount of time because gAudio
		is having too much problems on my computer ):,
	- Small boost to piXel!, one thing to do would be to write a sub-pixel/texel
		integer correction... hmmm too lazy right now,
	- Almost stable version gnark gnark gnark,
	- Oooops, minor bugs with sparks fixed,
	- Added Inverse Kinematik
		However IK chain's behavior is not the same as LW's one so it is unusable
		for organic motions... Cool for tentacles anyway (:
	- Light color affects Lens Flare color,
	- Rewrote transformation pipeline, objects can now morph and gizmo at the same time,
		also gave a major speedup, parenting objects almost cost nothing now...
	- Memory hole nearly vanished,
	- Silly bug with morph fixed...
	- Woohaa! TARGET fixed! Finally got to understand much things (:
		IK is too be done very soon! (Urch! headache...)
	- Multi-texture/Single-stage are now correctly interpreted,
	- Reduced alot memory requirements in all cases,
		added option to further reduce memory requirements but NOT in all cases!
	- Hmmm... No more Lightwave's samples crash now (=!
	- Improved Surface resolver Logic, hopefully no more bugs in it!
	- Minor fixes,
	- Added JPEG support to IMGmng using JPEGsrc6b garg... had to write
		custom IO manager a little bit crappy but it should work fine! (:
	- Implemented IFF support to IMGmng, (24 Bpp RAW/RLE for now !RLE NOT STABLE!)
	- Compatibility is almost 100%. Only 2 lightwave's samples crashes,
	- Oh shit! Loader improved! Hummer.lws doesn't fool it anymore [:!
	- Began to write some 'REAL' error handlers {; ,
	- Just watched 'a Bug's Life' and it ROXXXXXXXXXX! Arg! (=
	- Added .INI file to locate Lightwave's folder,
	- Enhancements to memory allocations...

- v1.7beta:
	- More solid clean up code... reduced the memory hole (;
	- Upgrade to my pic lib added BMP/FIX/FFC image format support,
	- Added some procedural map (Fractal Noise/Fractal Bumps/Crumple),
	- Removed a silly bug wich caused preca not to see sparks,
	- Improved transparency logic, [FOREMOST] is useless now,
	- Corrected some bugs in floating point modulo this should have fixed small problems
		with envelops using frame offset,
	- Rewrote motion loader,
	- Added XM and MOD support,
	- Multi-Scene GUI + gAUDIO output (MP3 support only! XM and MOD to come),
	- Colormap and alphamap are now merged in one ARGB d3d texture when possible
		to allow multi-texture transparency on non-multitexture capable board,
	- Maintenance in source code,
	- Rewrote features part of this doc.

- v1.6:
	MAJOR UPDATE!
	- Custom resolution (in lightwave' camera panel) correctly supported,
	- Added [FOREMOST] surface's keyword because of the multipass rendering
		using Direct3D, by default nX first renders opaque surfaces then transparent
		ones, with this keyword you can force an opaque surface to render AFTER
		a transparent one,
	- Removed MMX as it suxx hard (slower than straight pentium because my
		rasterizer strongly rely on FPU and doing EMMS all the time is F*****G
		SLOW! (:
	- Improved bilinear resizing,
	- Hum oh yeah, DirectX yet rewrote... seems that even after 10 months
		of DirectX coding you still learn every day \-:
	- Compatibility improved! Please try Lightwave tutorial's scenes (;
		THE RENDER IS STUNNING IN piXel!
	- Cleaned up the structures, saved some memory there and there,
	- Welcome home MOA! (: MOA is back,
	- Fixed problem with FLAT variant in generic rasterizer,
	- Rewrote the HOWTORENDER part of this doc,
	- Implemented the GENERIC RASTERIZER now software can perform EVERY rendertypes!
	- Corrected the loosy d3d bug wich made some rendertype unavailable...
		TNT additive + map + alpha now works (;
	- Corrected a stupid clipping error which caused the rasteriser to draw 639x479
		instead of 640x480 (:
	- Viewport are now 100% correct (Zoom Factor is correctly interpreted),
	- piXel has support for FLAT ( with optional lightsource ) + ALPHA texture
		+ optional ALPHA factor, (plus sotware WIDE support)
	- Negative image support for all levels,
	- Yahaaa! (: So cool i've found what I think is the BEST way to code
		D3D multi-texture stage... Now you can set every combination you want and
		if your hardware doesn't support one of them it will fall to the nearest
		possible! And the code takes only 1 page! (8 Pfffu so GREAT!
	- Implemented software wide texture map! Removes 256x256 texture size limitation,
		Specify [WM] in surface name to activate.
		WARNING!: Texture repeat WILL crash!
	- Fix: Some map memory wasn't deallocated because allocated more than once!
	- Implemented bilinear interpolation when resizing a texture,
	- Direct3D has no texture size limitation (except hardware's one),
	- Transparency texture level is now active (piXel support only for mapping
		or envirronment mapping no perspective),
	- Rewrote color conversions as DirectX does not perform them in soft in there
		is no hardware for them... MMX version also btw (;
	- YEAH! DirectX code seems to be solid as a rock now (; (is that truly possible?)
	- Added some MMX specific routines... Not so fast except for hires )<
	- Internal color scheme is now 32bits! RGB888,
	- Some cleanup in DirectX, color conversions are now leave to DirectDraw,
	- D3D implementation now support all possible texture formats except 8bits one,
	- Light color support with exact Lightwave's behavior.

- v1.5:
	MAJOR UPDATE!
	- Compatibility rate is near of 100%!
		Almost all scene can be loaded (even lightwave's tutorials), however as there
		is always many uncorrected polygon the rasteriser crash sometimes, some scenes
		also overcome the maX settings but i actually replay 34 out of 36 lightwave's
		tutorials! Including some heavy scenes upto 300000+ polygons YEAH!
	- Rewritten about 80% of clipping, lightning, face builder and some touch here
		and there to all polygon routines in order to implement structured vertex! Yes!
		I can't believe I did that (: ... So here what's new:
		- Faster clipping,
		- faster but still slow lights,
		- no more gouraud flashing bugs,
		- no more lightning bugs,
		- virtually unlimited texture levels with independent settings,
		- fast face constructor.
		The code is now really upgradable!

- v1.2:
	- Saved memory by only allocating used texture level...
	- Texture levels are now REALLY supported (:
		This mean you can map a bump using spherical and a color map
		using planar with both texture having different velocity and
		this will work! Yeah!
	- Linear fog is now supported in software (colored),
	- Cleanup in lightning code,
	- Speedup to polygons,
	- Lens flare support, you can specify a lens map you wish to use
		by using this keyword in light's name:
		[flare=<complete filename>]
	- Polygon size supported to make particle effects,
		i.e: any polygon size different from 1.0
				draw particle on each vertex sized in
				proportion of polygon size value,
	- Made most of the routine work outside a scene context,

	-=- CHANGES FROM X32 TO nXs3 -=-

- NEW ! for v5.24.99 :
	- Reduced memory requirement when doing multiscene,
	- Major changes within code to allow combining scenes without any restriction,
	- Removed a bug when loading GIZMO targets having more surfaces than GIZMO source,
	- Implement correct error return for DirectX inits,
	- No polygon bug removed today! It seems their bugs are getting very minor now ! (:

- NEW ! for v5.16.99 :
	- Rewrote texture code to be faster. I must now init DX before scene load ) : too bad !
	- Enhanced polygonal rasterization ! ALOT less bugs ! Non-planar & convexe should
		still be avoided anyway {8 but they shouldn't crash as often as before,
	- !ADDED! Bounding Boxes for optimal clippings,
	- !ADDED! Runtime statistic (only SOFT),
	- Lowered memory requirement from 100+mo to 10mo ! <Now i can load 40 scenes without any swapping ((:>
		Scenes now allocate everything on-the-fly, only screens and lookups are statics.
	- WAHOOOO ! ! ! ! Fucking optimisation to Smoothed normals calculations ! ! ! 
		ENJOY new loading times palz! This is ASTONISHINGLY faster ! (:
	- Cleaned lot of code, speeded up rotations, corrected a bug with negative modulo,
	- !ADDED! Pfff... MORPH-GIZMO !
	- Reorganised loader, once again more object oriented !
		This have cleaned the code ALOT ! It should now be really easy to implement new
		features (kweeel J ). Gizmo is almost finished and bones are on a very good way
	- Major optimisation (: Anyway THERE IS one restriction (would you call that a restriction ? (; )
		Just check the current statement and general rules for more infos <entry : !EDGEOPTIM !>,

- NEW ! for v4.29.99 :
	- Corrected two vicious bugs in my latest optimisations to Loader,
	- !ADDED! Software W-Buffering (32 bits) !
		must specify '[ZBUFFER]' in surface name to activate,
		Currently with :
			-Flat both lightsource & non-lightsource,
	- Maps bigger than 256x256 are now automatically reduced,
	- Percentage for scene loading,
	- Fully functional release now with preca enabled !,
	- !ADDED! Mapping + Chroma Keying + Alpha,
	- Surface's lighting restriction to specific light of the scene,
		Specified in name of surface by use of keyword 'LGT_x[+x][+...]'
		where x is the index of light in scene loading. Example :
			'LGT_1' or 'LGT_2+5+8+6'
	- !ADDED! Mapping + Gouraud + Chroma Keying,
	- !ADDED! Perspective mapping + Gouraud,
	- !ADDED! Mapping + Gouraud + Additive,
	- !ADDED! Envirronment mapping + Chroma Keying,
	- !ADDED! Mapping + Chroma Keying,

- NEW ! for v4.20.99 :
	- Major improvements to color levels ! ALPHA is now 100% correct !
	- Fixed luminosity differences between Soft and D3D,
	- More colors (64 shade levels versus 32 for previous version),
	- TOTALLY rewritten clippers!
		This removed a bunch of nasty bugs ! Hopefully there shouldn't be
		those random-'CLOSE/DEBUG' anymore ! (( :
		BTW : It fixed strange polys in 'sc1\land.lws',
	- !ADDED! Sub-pixel/sub-texel accuracy on all software renderers ! ! !
		This cost a large speed decrease but this really deserve it !
		Anyway rasterisation of polygons have been greatly improved
		by saving many memory access so the speed should not suffer that
		much with ALOT more correct renders with no holes beetwen polys
		and very accurate motions ! Think that 1'pixel-poly' do show now! ( :
	- D3D is now more accurate and should run better on more card.

- NEW ! for v4.7.99 :
	- Clipped envelops,
	- !ADDED! Automatic stretching of wrapping textures, now each textures
		is stretched to 256 pixels wide along its wrapping axis (if any),
		(does not apply to D3D renderer)
	- !ADDED! Object dissolve support ,
		Work with: _Gouraud (including Specular),
				_Flat both lightsourced or not (including Specular),
				_Non-Corrected mapping,
				_Non-Corrected mapping additive.
	- !ADDED! Support for TARGA24 & 32 RLE encoded,
	- Yes ! Yes ! Yes ! Finally removed that fucking loader bug wich made
		some scene only loading in debug compilation !
		BTW : gOds' TOYS scenes replays perfectly ! ( :
	- !ADDED! Edge&Cone angle envelop support for SpotLight,
	- !ADDED! Support for Cubic Image Map & Cylindrical Image Map.

- NEW ! for v4.5.99 :
	- Removed a minor bug in Direct3D cleanup,
	- First frame of scene also influence MOA so you can quickly visualize
		changes made to a scene without viewing it all!
	- Improved alot viewpoint skipping and also removed a vicious bug in it,
	- Yepaaa! Once again! Loader loads from 30 to 50% faster!
		old:23,930s -> new:17,216s
		old:3,452s -> legend, new:2,500s
		old:5,010s -> scenea, new:2,947s
		old :220,990s -> 16384 faces sphere gouraud, new : 112,304s
	- !ADDED !D3D Z-buffered output, < you got it Kinan! ( = >
		Mo W-buffer for the moment,
	- Removed Zcut problems with D3D,
	- Scene size is now entry dependent (i.e each scene in the list has its own size),
	- Added lens flare size panel (scene dependent also),
	- nX is no more fooled by the missing paths in Lightwave 5.6' scene file!
	- Corrected various HUGE bugzzz ( : .

- NEW ! since change (upto X32-4.2.99)
	- Totally rewritten loaders (expect a 2~3x gain factor in loading times),
	- Accurate mapping calculations,
	- Working DIRECT3D support (with benefits from multi-pass capable 3d accelerators),
		Texture format supported : RGB555, RGB565, BGR565 only!
	- Now fully object oriented,
		designed to be exportable! Only include "x3.h" and send
		user parameters to X3!!! ( = sooo good!
		implemented a scene list so you can nearly make a whole demo with the basic GUI!
	- Support for background Soundtrack (only MPEG-audio streams are supported for now),
	- Corrected end behavior' behavior ! ( :
	- Light's lens flare option now draw an additive scaled sprite (i.o.w : real time lens flare),
	- Lowered by about! 100%! memory requirements! Yes, this is true!,
	- Support for 16bit RLE-Encoded Targa added.

	-=- CHANGES FROM X3 TO X32 -=-

- NEW! for v2.4a:
	- ADDED! ALPHA transparency,
	- Gwa! I did deliberatly remove the parent sizing! Pfuuh... now fixed,
	- Texture width & height repeat supported (little loss of precision in some roots),
	- Finally properly re-implemented end behavior.
		It should works for EVERY supported motion types & envelops,
	- Et hop! MetaMorph is now supported for all CAPS (shaded morph is supported!),
	- Added camera focal envellop,
	- Finally had a fix for the camera banking bug! Thanxx gOzer!.

- NEW! for v2.4:
	- GREAT speed improvement to lights handling (go on now with your 32 spotlights ((: ),
	- Minor fixes to Direct3D,
	- Finally supporting 16bpp in 555 RGB mode (you coders know what i'm talking about),
	- Texture Fx (for now 4-way synched scrollings only),
	- !First SHazE 'Small Haze Engine' version!
		Allows for NULL SHazE objects to emit fog with parametrable velocity & direction!,
	- I still don't want to set version to 3.0 unless shadows are done...
		but i feel so lazy about this...
		even if major change have been done since 2.0!

- NEW! for v2.2c:
	- Glide support (fully functional),
	- Basic Direct3D support (MS guys should really learn how to write docs!),
	- Totally rewritten all Windows/DirectX inits,
	- Mixed color double-texture mapping added,
	- Wow! What a nice GUI you got here (8!,
	- FIXED: Camera switches now works.

- NEW! for v2.2:
	- Discarded Z-Buffer shadowmaps as they sucked,
		but v2.3 might feature PolyLook Realtime shadows,
	- RAYTRACED shadowmaps (support for ALL usual light types SPOT-LINEAR-POINT),
		ONLY for PRECALCULATED shadow maps! DISABLED.
	- MAJOR Cleanup in code,
		a step further in encapsulation. More structured now,
	- Alot improved GUI! Allowing for lightning-speed Lightwave~X3 previewing,
	- Faster loading, less inits, more power (-; .

- NEW! for v2.0:
	- No more greyscale suckin' conversion for double-texture maps.
	- Support for precalculated Z-Buffer SHADOWMAPS!
		These are calculated upon ALL spotlights casting shadows in scene.

- NEW! for v1.9:
	- Major upgrade to light handling:
		- SpotLight with both Cone Angle & Edge Angle 100% !CORRECT!,
		- Ambient Intensity support envelop,
		- Light intensity support envelop, (all types)
		- Light range support envelop, (Spot and Point lights only to conform to LightWave)
		- Unlimited number of lights,
		- Distant light.
		- Still (and probably never) support for light color )):, boooh...

- NEW! for v1.8:
	FORGET YOUR OLD 'LIGHT-IS-THE-CAMERA' shaded polygons!
	Primary lights support:
		-Point light is correct but Spot Light still need some work.

- NEW! for v1.7:
	- MaxSmoothingAngle feature added! Very useful! Normals-based shading looks ALOT better.
		(BTW: Lightwave SDK is WRONG as the IEEE is already in radian and not in degree)
	- Preca is now calculated with zBuffer (v1.7 :: suggestion from the din-O),

- NEW! for v1.6:
	- Preca enhanced by using 3 different sorting (discarded),
	- Preca. added!
		Onto fabulous framerate now!,

- NEW! for v1.5:
	- A GUI!!! Unbelievable but TRUE! a Working WINDOWS-Like GUI
		(ugly as possible but always useful).

## ** OBSOLETE Revisions... **
  
- Rev: 1.5	- Finally Z-corrected sutherland implemented!
- Rev: 1.4	- Rewritten Z-cut & sutherland pass to be more flexible,
				better and easier implementation of surface types,
			- Mapping support!.
- Rev: 1.2	- Added 100% correct rotations matrix (LIGHTWAVE model),
				camera's banking still looks wrong but i'm not sure...
				(2.0: I was RIGHT plop! ((:)
- Rev: 1.1	- Parent objects added!,
			- Support Null Object command,
			- Full pivot point support.
- Rev: 1.0	- Working Z-Cut,
			- Flat no surface ]: .
- Rev:<1.0	- Rotating points ((((: hahahaha!
