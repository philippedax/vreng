const char DTD[] = "\
<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
\n\
<!ENTITY Pub-Status \"This is a pre-release of the specification.\">\n\
\n\
<!-- global declaration, there must be one head and one scene -->\n\
<!-- VRE -->\n\
<!ELEMENT vre		(head, scene)>\n\
<!ATTLIST vre		profile	NMTOKEN #IMPLIED>\n\
<!ATTLIST vre		version	NMTOKEN #IMPLIED>\n\
\n\
<!-- we want at least one meta information about the file -->\n\
<!-- HEAD -->\n\
<!ELEMENT head		(meta+)>\n\
\n\
<!-- META -->\n\
<!-- meta could contain lot of fields as \"author\" \"filename\" etc. -->\n\
<!ELEMENT meta		EMPTY>\n\
<!ATTLIST meta		name	(url | filename | description | keyword | author | date | created | revised) #IMPLIED>\n\
<!ATTLIST meta		content	CDATA #IMPLIED>\n\
<!ATTLIST meta		http-equiv (refresh | static) #IMPLIED>\n\
\n\
<!-- all possible objects we want at least one element -->\n\
<!-- SCENE -->\n\
<!ELEMENT scene (\n\
		accessory | android | animator | animx3d | aoi | area |\n\
		bgcolor | board | book | bumper | button | \n\
		cauldron | clip | clock | cloud | cloth |\n\
		doc | door | dress |\n\
		entry | escalator |\n\
		fire | firework | flag | fountain | ftp |\n\
		gate | ground | guide | guy | \n\
		haid | hairs | halo | hat | host |\n\
		lift | light | local |\n\
		mech | mirage | mirror | model | movie | music |\n\
		particle | plane |\n\
		rain | river |\n\
		slider | smoke | snow | spiral | stair | stars | step | sun |\n\
		terrain | text | thing | thunder | travelator |\n\
		vjc | vnc | vrelet | \n\
		wall | walls | water | waterfall | web | widget | wind | wings\n\
		)+\n\
>\n\
\n\
<!-- alphabetic sort of main token -->\n\
\n\
<!ELEMENT accessory	(solid)>\n\
<!ATTLIST accessory	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST accessory	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST accessory	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST accessory	speed	NMTOKEN \"0.1\">\n\
<!ATTLIST accessory	slowx	NMTOKEN \"50\">\n\
<!ATTLIST accessory	slowy	NMTOKEN \"50\">\n\
<!ATTLIST accessory	slowz	NMTOKEN \"50\">\n\
<!ATTLIST accessory	shiftx	NMTOKEN \"0\">\n\
<!ATTLIST accessory	shifty	NMTOKEN \"0\">\n\
<!ATTLIST accessory	shiftz	NMTOKEN \"0\">\n\
\n\
<!ELEMENT android	EMPTY>\n\
<!ATTLIST android	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST android	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST android	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST android	body	CDATA #REQUIRED>\n\
<!ATTLIST android	face	CDATA #IMPLIED>\n\
<!ATTLIST android	color	NMTOKENS #IMPLIED>\n\
<!ATTLIST android	server	NMTOKEN #IMPLIED>\n\
\n\
<!ELEMENT animator	(solid+)>\n\
<!ATTLIST animator	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST animator	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST animator	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST animator	state	(0 | 1) \"0\">\n\
<!ATTLIST animator	begin	NMTOKEN \"0\">\n\
<!ATTLIST animator	ttl	NMTOKEN \"0\">\n\
\n\
<!ELEMENT animx3d	EMPTY>\n\
<!ATTLIST animx3d	pos	CDATA #REQUIRED>\n\
<!ATTLIST animx3d	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST animx3d	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST animx3d	url	CDATA #REQUIRED>\n\
<!ATTLIST animx3d	dim	CDATA #IMPLIED>\n\
\n\
<!ELEMENT aoi		(solid?)>\n\
<!ATTLIST aoi		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST aoi		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST aoi		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST aoi		channel	CDATA #REQUIRED>\n\
\n\
<!ELEMENT area		EMPTY>\n\
<!ATTLIST area		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST area		name	NMTOKEN #REQUIRED>\n\
<!ATTLIST area		descr	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT bgcolor	EMPTY>\n\
<!ATTLIST bgcolor	color	NMTOKENS #REQUIRED>\n\
\n\
<!ELEMENT board		(solid)>\n\
<!ATTLIST board		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST board		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST board		descr	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT book		(solid)>\n\
<!ATTLIST book		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST book		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST book		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST book		url	CDATA #REQUIRED>\n\
\n\
<!ELEMENT bumper	(solid)>\n\
<!ATTLIST bumper	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST bumper	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST bumper	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST bumper	forcex	NMTOKEN \"1\">\n\
<!ATTLIST bumper	forcey	NMTOKEN \"1\">\n\
<!ATTLIST bumper	forcez	NMTOKEN \"1\">\n\
\n\
<!ELEMENT button	(solid)>\n\
<!ATTLIST button	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST button	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST button	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST button	use	NMTOKEN #REQUIRED>\n\
<!ATTLIST button	method0	NMTOKEN #REQUIRED>\n\
<!ATTLIST button	method1	NMTOKEN #IMPLIED>\n\
<!ATTLIST button	state	(0 | 1) \"0\">\n\
\n\
<!ELEMENT cauldron	(solid)>\n\
<!ATTLIST cauldron	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST cauldron	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST cauldron	descr	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT clip		(solid+)>\n\
<!ATTLIST clip		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST clip		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST clip		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST clip		url	CDATA #REQUIRED>\n\
\n\
<!ELEMENT clock		(solid)>\n\
<!ATTLIST clock		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST clock		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST clock		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST clock		needle	(s | m | h | sec | min | hour) #REQUIRED>\n\
\n\
<!ELEMENT cloud		EMPTY>\n\
<!ATTLIST cloud		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST cloud		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST cloud		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST cloud		number	NMTOKEN \"200\">\n\
<!ATTLIST cloud		speed	NMTOKEN \"0.025\">\n\
\n\
<!ELEMENT cloth		EMPTY>\n\
<!ATTLIST cloth		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST cloth		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST cloth		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST cloth		article	(halo | hat | dress | wings) #REQUIRED>\n\
<!ATTLIST cloth		color	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT doc		(solid)>\n\
<!ATTLIST doc		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST doc		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST doc		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST doc		url	CDATA #REQUIRED>\n\
\n\
<!ELEMENT door		(solid)>\n\
<!ATTLIST door		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST door		name	NMTOKEN #REQUIRED>\n\
<!ATTLIST door		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST door		open	NMTOKEN #REQUIRED>\n\
<!ATTLIST door		close	NMTOKEN #REQUIRED>\n\
<!ATTLIST door		speed	NMTOKEN \"1\">\n\
\n\
<!ELEMENT dress		(solid+)>\n\
<!ATTLIST dress		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST dress		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST dress		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST dress		model	(spires | bands | roses) #IMPLIED>\n\
\n\
<!ELEMENT entry		EMPTY>\n\
<!ATTLIST entry		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST entry		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST entry		descr	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT escalator	(solid+)>\n\
<!ATTLIST escalator	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST escalator	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST escalator	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST escalator	dir	(up | down | horizontal) \"up\">\n\
<!ATTLIST escalator	height	NMTOKEN \"0\">\n\
<!ATTLIST escalator	length	NMTOKEN \"0\">\n\
<!ATTLIST escalator	speed	NMTOKEN \"0.5\">\n\
\n\
<!ELEMENT fire		EMPTY>\n\
<!ATTLIST fire		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST fire		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST fire		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST fire		number	NMTOKEN \"1000\">\n\
<!ATTLIST fire		speed	NMTOKEN \"1\">\n\
<!ATTLIST fire		sound	NMTOKEN \"0\">\n\
\n\
<!ELEMENT firework	(solid)>\n\
<!ATTLIST firework	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST firework	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST firework	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST firework	size	NMTOKEN \"1\">\n\
<!ATTLIST firework	speed	NMTOKEN \"0\">\n\
<!ATTLIST firework	ttl	NMTOKEN \"20\">\n\
<!ATTLIST firework	number	NMTOKEN \"400\">\n\
\n\
<!ELEMENT flag		EMPTY>\n\
<!ATTLIST flag		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST flag		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST flag		descr	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT fountain	EMPTY>\n\
<!ATTLIST fountain	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST fountain	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST fountain	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST fountain	ground	NMTOKEN \"0\">\n\
<!ATTLIST fountain	size	NMTOKEN \"1\">\n\
<!ATTLIST fountain	speed	NMTOKEN \"5\">\n\
<!ATTLIST fountain	flow	NMTOKEN \"500\">\n\
<!ATTLIST fountain	number	NMTOKEN \"5000\">\n\
\n\
<!ELEMENT ftp		(solid)>\n\
<!ATTLIST ftp		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST ftp		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST ftp		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST ftp		url	CDATA #REQUIRED>\n\
\n\
<!ELEMENT gate		(solid+)>\n\
<!ATTLIST gate		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST gate		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST gate		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST gate		url	CDATA #REQUIRED>\n\
<!ATTLIST gate		channel	CDATA #IMPLIED>\n\
<!ATTLIST gate		entry	NMTOKENS #IMPLIED>\n\
<!ATTLIST gate		mode	(auto | link) #IMPLIED>\n\
\n\
<!ELEMENT ground	(solid+)>\n\
<!ATTLIST ground	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST ground	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST ground	descr	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT guide		EMPTY>\n\
<!ATTLIST guide		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST guide		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST guide		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST guide		path	NMTOKENS #REQUIRED>\n\
<!ATTLIST guide		color	NMTOKENS #IMPLIED>\n\
<!ATTLIST guide		mode	(one-way | round-trip | perpetual) \"round-trip\">\n\
\n\
<!ELEMENT guy		EMPTY>\n\
<!ATTLIST guy		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST guy		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST guy		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST guy		url	CDATA #REQUIRED>\n\
<!ATTLIST guy		anim	(0 | 1) \"0\">\n\
<!ATTLIST guy		walk	(0 | 1) \"0\">\n\
<!ATTLIST guy		sex	(0 | 1) \"0\">\n\
<!ATTLIST guy		skin	NMTOKEN #IMPLIED>\n\
<!ATTLIST guy		bust	NMTOKEN #IMPLIED>\n\
<!ATTLIST guy		legs	NMTOKEN #IMPLIED>\n\
<!ATTLIST guy		feet	NMTOKEN #IMPLIED>\n\
\n\
<!ELEMENT hairs		EMPTY>\n\
<!ATTLIST hairs		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST hairs		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST hairs		descr	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT halo		(solid+)>\n\
<!ATTLIST halo		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST halo		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST halo		descr	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT hat		(solid+)>\n\
<!ATTLIST hat		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST hat		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST hat		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST hat		model	(highform | boiler | cardinal | chinese | country) #IMPLIED>\n\
\n\
<!ELEMENT head		EMPTY>\n\
<!ATTLIST head		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST head		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST head		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST head		url	CDATA #REQUIRED>\n\
<!ATTLIST head		scale	NMTOKEN \"1\">\n\
<!ATTLIST head		color	NMTOKENS \"1 .75 .7\">\n\
\n\
<!ELEMENT host		(solid+)>\n\
<!ATTLIST host		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST host		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST host		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST host		host	NMTOKEN #REQUIRED>\n\
\n\
<!ELEMENT lift		(solid+)>\n\
<!ATTLIST lift		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST lift		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST lift		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST lift		top	NMTOKEN #REQUIRED>\n\
<!ATTLIST lift		bot	NMTOKEN #REQUIRED>\n\
<!ATTLIST lift		height	NMTOKEN #REQUIRED>\n\
<!ATTLIST lift		speed	NMTOKEN \"1\">\n\
\n\
<!ELEMENT light		EMPTY>\n\
<!ATTLIST light		pos	NMTOKENS #IMPLIED>\n\
<!ATTLIST light		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST light		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST light		fog	NMTOKEN \"0\">\n\
<!ATTLIST light		color	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT local		(local | model | transform)*>\n\
<!ATTLIST local		descr	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT mech		EMPTY>\n\
<!ATTLIST mech		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST mech		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST mech		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST mech		anim	(0 | 1) \"0\">\n\
<!ATTLIST mech		walk	(0 | 1) \"0\">\n\
\n\
<!ELEMENT mirage	(solid+)>\n\
<!ATTLIST mirage	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST mirage	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST mirage	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST mirage	mode	(turn | roll | tilt | orbit | circular | refresh | volatile) #IMPLIED>\n\
<!ATTLIST mirage	aspeed	NMTOKEN \"-0.2\">\n\
<!ATTLIST mirage	azspeed	NMTOKEN \"-0.2\">\n\
<!ATTLIST mirage	ayspeed	NMTOKEN \"-0.2\">\n\
<!ATTLIST mirage	axspeed	NMTOKEN \"-0.2\">\n\
<!ATTLIST mirage	radius	NMTOKEN #IMPLIED>\n\
<!ATTLIST mirage	flare	NMTOKEN #IMPLIED>\n\
\n\
<!ELEMENT mirror	(solid)>\n\
<!ATTLIST mirror	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST mirror	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST mirror	descr	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT model		EMPTY>\n\
<!ATTLIST model		pos	NMTOKENS #IMPLIED>\n\
<!ATTLIST model		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST model		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST model		url	CDATA #REQUIRED>\n\
<!ATTLIST model		scale	NMTOKEN \"1\">\n\
<!ATTLIST model		tex	CDATA #IMPLIED>\n\
<!ATTLIST model		sound	CDATA #IMPLIED>\n\
<!ATTLIST model		color	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT movie		(solid+)>\n\
<!ATTLIST movie		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST movie		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST movie		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST movie		url	CDATA #REQUIRED>\n\
<!ATTLIST movie		rate	NMTOKEN \"10\">\n\
\n\
<!ELEMENT music		(solid+)>\n\
<!ATTLIST music		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST music		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST music		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST music		url	CDATA #REQUIRED>\n\
<!ATTLIST music		state	(play | loop) #IMPLIED>\n\
\n\
<!ELEMENT particle	EMPTY>\n\
<!ATTLIST particle	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST particle	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST particle	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST particle	system	(firework | waterfall | fountain | rain | snow) \"waterfall\">\n\
<!ATTLIST particle	ground	NMTOKEN \"0\">\n\
<!ATTLIST particle	size	NMTOKEN \"1\">\n\
<!ATTLIST particle	speed	NMTOKEN \"5\">\n\
<!ATTLIST particle	flow	NMTOKEN \"500\">\n\
<!ATTLIST particle	number	NMTOKEN \"5000\">\n\
\n\
<!ELEMENT plane		(solid+)>\n\
<!ATTLIST plane		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST plane		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST plane		descr	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT rain		EMPTY>\n\
<!ATTLIST rain		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST rain		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST rain		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST rain		ground	NMTOKEN \"0\">\n\
<!ATTLIST rain		size	NMTOKEN \"1\">\n\
<!ATTLIST rain		speed	NMTOKEN \"10\">\n\
<!ATTLIST rain		flow	NMTOKEN \"500\">\n\
<!ATTLIST rain		number	NMTOKEN \"5000\">\n\
\n\
<!ELEMENT river		EMPTY>\n\
<!ATTLIST river		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST river		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST river		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST river		waves	NMTOKEN \"10\">\n\
<!ATTLIST river		width	NMTOKEN \"20\">\n\
<!ATTLIST river		depth	NMTOKEN \"5\">\n\
<!ATTLIST river		scale	NMTOKEN \"1\">\n\
<!ATTLIST river		color	NMTOKENS \"0.1 0.2 0.3 1\">\n\
\n\
<!ELEMENT slider	(solid)>\n\
<!ATTLIST slider	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST slider	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST slider	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST slider	incrx	NMTOKEN \"0\">\n\
<!ATTLIST slider	incry	NMTOKEN \"0\">\n\
<!ATTLIST slider	incrz	NMTOKEN \"0\">\n\
\n\
<!ELEMENT smoke		EMPTY>\n\
<!ATTLIST smoke		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST smoke		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST smoke		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST smoke		number	NMTOKEN \"1000\">\n\
<!ATTLIST smoke		speed	NMTOKEN \"1\">\n\
\n\
<!ELEMENT snow		EMPTY>\n\
<!ATTLIST snow		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST snow		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST snow		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST snow		ground	NMTOKEN \"0\">\n\
<!ATTLIST snow		size	NMTOKEN \"2\">\n\
<!ATTLIST snow		speed	NMTOKEN \"5\">\n\
<!ATTLIST snow		flow	NMTOKEN \"500\">\n\
<!ATTLIST snow		number	NMTOKEN \"5000\">\n\
\n\
<!ELEMENT spiral	(solid+)>\n\
<!ATTLIST spiral	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST spiral	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST spiral	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST spiral	height	NMTOKEN \"0\">\n\
\n\
<!ELEMENT stair		(solid+)>\n\
<!ATTLIST stair		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST stair		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST stair		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST stair		height	NMTOKEN \"0\">\n\
\n\
<!ELEMENT stars		EMPTY>\n\
<!ATTLIST stars		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST stars		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST stars		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST stars		number	NMTOKEN \"200\">\n\
<!ATTLIST stars		speed	NMTOKEN \"10\">\n\
<!ATTLIST stars		warp	NMTOKEN \"20\">\n\
<!ATTLIST stars		maxpos	NMTOKEN \"4000\">\n\
\n\
<!ELEMENT step		(solid+)>\n\
<!ATTLIST step		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST step		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST step		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST step		mode	(stair | spiral | escalator | travelator) #IMPLIED>\n\
<!ATTLIST step		dir	(up | down | horizontal) \"up\">\n\
<!ATTLIST step		height	NMTOKEN \"0\">\n\
<!ATTLIST step		length	NMTOKEN \"0\">\n\
<!ATTLIST step		speed	NMTOKEN \"0.1\">\n\
\n\
<!ELEMENT sun		EMPTY>\n\
<!ATTLIST sun		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST sun		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST sun		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST sun		scale	NMTOKEN \"1\">\n\
\n\
<!ELEMENT terrain	EMPTY>\n\
<!ATTLIST terrain	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST terrain	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST terrain	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST terrain	level	NMTOKEN \"6\">\n\
<!ATTLIST terrain	height	NMTOKEN \"0.1\">\n\
<!ATTLIST terrain	div	NMTOKEN \"1.85\">\n\
<!ATTLIST terrain	scale	NMTOKEN \"100\">\n\
<!ATTLIST terrain	color	NMTOKEN \"0 1 0 1\">\n\
\n\
<!ELEMENT text		(#PCDATA)>\n\
<!ATTLIST text		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST text		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST text		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST text		font	CDATA #IMPLIED>\n\
<!ATTLIST text		color	NMTOKENS #IMPLIED>\n\
<!ATTLIST text		scale	NMTOKEN \"1\">\n\
<!ATTLIST text		verso	(0 | 1) \"0\">\n\
\n\
<!ELEMENT thing		(solid+)>\n\
<!ATTLIST thing		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST thing		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST thing		descr	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT thunder	EMPTY>\n\
<!ATTLIST thunder	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST thunder	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST thunder	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST thunder	number	NMTOKEN \"1\">\n\
<!ATTLIST thunder	period	NMTOKEN \"2\">\n\
\n\
<!ELEMENT transform	EMPTY>\n\
<!ATTLIST transform	rotation	NMTOKENS #IMPLIED>\n\
<!ATTLIST transform	translation	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT travelator	(solid+)>\n\
<!ATTLIST travelator	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST travelator	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST travelator	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST travelator	length	NMTOKEN \"0\">\n\
<!ATTLIST travelator	speed	NMTOKEN \"0.5\">\n\
\n\
<!ELEMENT vjc		EMPTY>\n\
<!ATTLIST vjc		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST vjc		server	NMTOKEN #REQUIRED>\n\
\n\
<!ELEMENT vnc		(solid+)>\n\
<!ATTLIST vnc		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST vnc		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST vnc		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST vnc		server	NMTOKEN #REQUIRED>\n\
<!ATTLIST vnc		port	NMTOKEN \"5901\">\n\
<!ATTLIST vnc		passwd	NMTOKEN #IMPLIED>\n\
\n\
<!ELEMENT vrelet	(solid)>\n\
<!ATTLIST vrelet	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST vrelet	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST vrelet	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST vrelet	class	NMTOKEN #REQUIRED>\n\
<!ATTLIST vrelet	base	CDATA #IMPLIED>\n\
<!ATTLIST vrelet	fx	NMTOKEN \"1\">\n\
<!ATTLIST vrelet	fy	NMTOKEN \"1\">\n\
<!ATTLIST vrelet	incrx	NMTOKEN \"0\">\n\
<!ATTLIST vrelet	incry	NMTOKEN \"0\">\n\
\n\
<!ELEMENT wall		(solid+)>\n\
<!ATTLIST wall		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST wall		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST wall		descr	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT walls		EMPTY>\n\
<!ATTLIST walls		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST walls		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST walls		url	CDATA #REQUIRED>\n\
\n\
<!ELEMENT water		(solid)>\n\
<!ATTLIST water		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST water		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST water		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST water		amplitude	NMTOKEN \"1\">\n\
<!ATTLIST water		freq	NMTOKEN \"1\">\n\
<!ATTLIST water		phase	NMTOKEN \"1\">\n\
<!ATTLIST water		height	NMTOKEN #IMPLIED>\n\
\n\
<!ELEMENT waterfall	EMPTY>\n\
<!ATTLIST waterfall	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST waterfall	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST waterfall	descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST waterfall	ground	NMTOKEN \"0\">\n\
<!ATTLIST waterfall	size	NMTOKEN \"1\">\n\
<!ATTLIST waterfall	speed	NMTOKEN \"5\">\n\
<!ATTLIST waterfall	flow	NMTOKEN \"500\">\n\
<!ATTLIST waterfall	number	NMTOKEN \"5000\">\n\
\n\
<!ELEMENT web		(solid+)>\n\
<!ATTLIST web		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST web		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST web		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST web		legend	NMTOKENS #IMPLIED>\n\
<!ATTLIST web		url	CDATA #REQUIRED>\n\
\n\
<!ELEMENT widget	(solid+)>\n\
<!ATTLIST widget	pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST widget	name	NMTOKEN #IMPLIED>\n\
<!ATTLIST widget	descr	NMTOKENS #IMPLIED>\n\
\n\
<!ELEMENT wind		EMPTY>\n\
<!ATTLIST wind		orient	NMTOKENS #REQUIRED>\n\
<!ATTLIST wind		speed	NMTOKENS #REQUIRED>\n\
\n\
<!ELEMENT wings		(solid+)>\n\
<!ATTLIST wings		pos	NMTOKENS #REQUIRED>\n\
<!ATTLIST wings		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST wings		descr	NMTOKENS #IMPLIED>\n\
<!ATTLIST wings		model	(butterfly | libellule | angel | eagle) #IMPLIED>\n\
\n\
\n\
<!-- sub-structure of main tokens -->\n\
\n\
<!-- SOLID -->\n\
<!ELEMENT solid		(frame*)>\n\
<!ATTLIST solid		a	NMTOKEN \"1\">\n\
<!ATTLIST solid		alpha	NMTOKEN \"1\">\n\
<!ATTLIST solid		amb	NMTOKENS #IMPLIED>\n\
<!ATTLIST solid		ambient	NMTOKENS #IMPLIED>\n\
<!ATTLIST solid		blink	(1) \"1\">\n\
<!ATTLIST solid		dif	NMTOKENS #IMPLIED>\n\
<!ATTLIST solid		diffuse	NMTOKENS #IMPLIED>\n\
<!ATTLIST solid		dim	NMTOKENS #IMPLIED>\n\
<!ATTLIST solid		emi	NMTOKENS #IMPLIED>\n\
<!ATTLIST solid		emission NMTOKENS #IMPLIED>\n\
<!ATTLIST solid		fa	(1) \"1\">\n\
<!ATTLIST solid		face	(1) \"1\">\n\
<!ATTLIST solid		fog	NMTOKEN \"0\">\n\
<!ATTLIST solid		frames	NMTOKEN \"1\">\n\
<!ATTLIST solid		h	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		height	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		l	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		length	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		model	CDATA #IMPLIED>\n\
<!ATTLIST solid		name	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		ptsize	NMTOKEN \"1\">\n\
<!ATTLIST solid		r	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		radius	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		radius2	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		radius3	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		rb	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		rc	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		re	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		rel	NMTOKENS #IMPLIED>\n\
<!ATTLIST solid		ri	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		rt	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		s	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		sc	NMTOKEN \"1\">\n\
<!ATTLIST solid		scale	NMTOKEN \"1\">\n\
<!ATTLIST solid		scalex	NMTOKEN \"1\">\n\
<!ATTLIST solid		scaley	NMTOKEN \"1\">\n\
<!ATTLIST solid		scalez	NMTOKEN \"1\">\n\
<!ATTLIST solid		shape	(bbox | box | bsphere | sphere | cylinder | cone | torus | rect | disk | line | pyramid | triangle | circle | ellipse | helix | wheel | pt | point | statue | model | cross | dsphere | saucer | hat | man | guy | car | teapot) \"box\">\n\
<!ATTLIST solid		shi	NMTOKEN \"20\">\n\
<!ATTLIST solid		shininess NMTOKEN \"20\">\n\
<!ATTLIST solid		side	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		size	NMTOKENS #IMPLIED>\n\
<!ATTLIST solid		sk	NMTOKEN \"16\">\n\
<!ATTLIST solid		sl	NMTOKEN \"16\">\n\
<!ATTLIST solid		slices	NMTOKEN \"16\">\n\
<!ATTLIST solid		stacks	NMTOKEN \"16\">\n\
<!ATTLIST solid		spokes	NMTOKEN \"12\">\n\
<!ATTLIST solid		sp	NMTOKEN \"12\">\n\
<!ATTLIST solid		spe	NMTOKENS #IMPLIED>\n\
<!ATTLIST solid		specular NMTOKENS #IMPLIED>\n\
<!ATTLIST solid		st	(0 | 1 | 2) \"0\">\n\
<!ATTLIST solid		style	(0 | 1 | 2) \"0\">\n\
<!ATTLIST solid		t	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		texture CDATA #IMPLIED>\n\
<!ATTLIST solid		texture_r CDATA #IMPLIED>\n\
<!ATTLIST solid		thick	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid	 	tx	CDATA #IMPLIED>\n\
<!ATTLIST solid		tx_back	CDATA #IMPLIED>\n\
<!ATTLIST solid		tx_front CDATA #IMPLIED>\n\
<!ATTLIST solid		tx_left	CDATA #IMPLIED>\n\
<!ATTLIST solid		tx_right CDATA #IMPLIED>\n\
<!ATTLIST solid		tx_top	CDATA #IMPLIED>\n\
<!ATTLIST solid		tx_bottom CDATA #IMPLIED>\n\
<!ATTLIST solid		txr	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		url	CDATA #IMPLIED>\n\
<!ATTLIST solid		xn	CDATA #IMPLIED>\n\
<!ATTLIST solid		xnr	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		xnrs	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		xnrt	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		xp	CDATA #IMPLIED>\n\
<!ATTLIST solid		xpr	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		xprs	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		xprt	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		yn	CDATA #IMPLIED>\n\
<!ATTLIST solid		ynr	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		ynrs	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		ynrt	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		yp	CDATA #IMPLIED>\n\
<!ATTLIST solid		ypr	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		yprs	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		yprt	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		zn	CDATA #IMPLIED>\n\
<!ATTLIST solid		znr	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		znrs	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		znrt	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		zp	CDATA #IMPLIED>\n\
<!ATTLIST solid		zpr	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		zprs	NMTOKEN #IMPLIED>\n\
<!ATTLIST solid		zprt	NMTOKEN #IMPLIED>\n\
\n\
<!-- FRAME -->\n\
<!ELEMENT frame		EMPTY>\n\
<!ATTLIST frame		a	NMTOKEN \"1\">\n\
<!ATTLIST frame		alpha	NMTOKEN \"1\">\n\
<!ATTLIST frame		amb	NMTOKENS #IMPLIED>\n\
<!ATTLIST frame		bf	NMTOKEN \"0\">\n\
<!ATTLIST frame		beginframe NMTOKEN \"0\">\n\
<!ATTLIST frame		dif	NMTOKENS #IMPLIED>\n\
<!ATTLIST frame		diffuse NMTOKENS #IMPLIED>\n\
<!ATTLIST frame		endframe NMTOKEN #IMPLIED>\n\
<!ATTLIST frame		h	NMTOKEN #IMPLIED>\n\
<!ATTLIST frame		height	NMTOKEN #IMPLIED>\n\
<!ATTLIST frame		model	CDATA #IMPLIED>\n\
<!ATTLIST frame		rb	NMTOKEN #IMPLIED>\n\
<!ATTLIST frame		rt	NMTOKEN #IMPLIED>\n\
<!ATTLIST frame		scale	NMTOKEN \"1\">\n\
<!ATTLIST frame		shape	NMTOKEN #REQUIRED>\n\
<!ATTLIST frame		spe	NMTOKENS #IMPLIED>\n\
<!ATTLIST frame		texture	CDATA #IMPLIED>\n\
<!ATTLIST frame		tex	CDATA #IMPLIED>\n\
<!ATTLIST frame		tx	CDATA #IMPLIED>\n\
<!ATTLIST frame		url	CDATA #IMPLIED>\n\
\n\
<!-- -->\n\
\n\
";
