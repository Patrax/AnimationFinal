#include "ArticulatedFigure.h"

////////////////////////////////////////////////////////////////////////////////////////
//	Constructor
////////////////////////////////////////////////////////////////////////////////////////

ArticulatedFigure::ArticulatedFigure(void){
	root = NULL;
	frameCount = 0;
}

////////////////////////////////////////////////////////////////////////////////////////
//	Destructor
////////////////////////////////////////////////////////////////////////////////////////

ArticulatedFigure::~ArticulatedFigure(void){
	delete root;
}

////////////////////////////////////////////////////////////////////////////////////////
//	Recursively draws all the links in the articulated body. 
////////////////////////////////////////////////////////////////////////////////////////

void ArticulatedFigure::draw(int flags){
	Matrix4x4 toWorld;
	double values[16];
	extern void drawFrame(float size);
	extern void drawArrow(float size, float x, float y, float th);

	if (!root) return;                       // make sure there is a skeleton
	QRoot.getRotationMatrix(&toWorld);       // convert root quaternion (QRoot) to a rotation matrix (toWorld)
	toWorld.getOGLValues(values);            // convert rotation matrix (toWorld) to an array for OpenGL

	double dx = values[8];
	double dz = values[10];
//	double ilen = 1.0/sqrt(dx*dx+dz*dz);
	float th = float(180.0/3.1416*atan2(dz,dx));

	drawArrow(10.0f, float(PRoot.x), float(PRoot.z), th);    // draw ground arrow
	glTranslated(PRoot.x, PRoot.y, PRoot.z);  // apply root translation
	glMultMatrixd(values);	                  // apply root rotation
//	drawFrame(10.0);                          // draw root coordinate frame
	root->drawChildren(flags);                // draw the children
}

////////////////////////////////////////////////////////////////////////////////////////
//	Recursively draws all the links in the articulated body (relative)
////////////////////////////////////////////////////////////////////////////////////////

void ArticulatedFigure::drawR(int flags){
	Matrix4x4 toWorld;
	double values[16];
	extern void drawFrame(float size);
	extern void drawArrow(float size, float x, float y, float th);

	if (!root) return;                       // make sure there is a skeleton
	QRoot.getRotationMatrix(&toWorld);       // convert root quaternion (QRoot) to a rotation matrix (toWorld)
	toWorld.getOGLValues(values);            // convert rotation matrix (toWorld) to an array for OpenGL

	double dx = values[8];
	double dz = values[10];
//	double ilen = 1.0/sqrt(dx*dx+dz*dz);
	float th = float(180.0/3.1416*atan2(dz,dx));

	drawArrow(10.0f, float(Cx), float(Cz), float(Cth));    // draw ground arrow
//	drawArrow(10.0f, float(PRoot.x), float(PRoot.z), th);    // draw ground arrow
	glTranslated(Cx, PRoot.y, Cz);  // apply root translation
//	glTranslated(PRoot.x, PRoot.y, PRoot.z);  // apply root translation
	glRotatef(float(Cth - Wth), 0,-1,0);
	glMultMatrixd(values);	                  // apply root rotation
//	drawFrame(10.0);                          // draw root coordinate frame
	root->drawChildren(flags);                // draw the children
}

////////////////////////////////////////////////////////////////////////////////////////
//	Reads an AMC motion file into the block of memory pointed to by "data"
////////////////////////////////////////////////////////////////////////////////////////

void ArticulatedFigure::readMotion(char *fname){

	char buffer[100];
	char linkName[100];
	Link *link;
	int temp=0;
	int ndof=0;
	int indx=0;
	int startCount = frameCount;

	FILE *fp = fopen(fname,"rb");
	fgets(buffer,100,fp);
	while (1) {       // for each frame
		if (feof(fp)) break;
		if (sscanf(buffer, "%d", &temp)!=1) {
			fgets(buffer,100,fp);    // read next line
			continue;                // then test again
		}
		fgets(buffer,100,fp);
		ndof = 0;
		while(1) {       // for each text line belonging to frame
			if (sscanf(buffer, "%d", &temp)==1) break;       // reached end of frame
			std::vector<char*> tokens = getTokens(buffer);   // read and parse tokens
			int ntokens = int(tokens.size());
			if (feof(fp)) break;
			  // record DOFs 

			sscanf(tokens[0], "%s", linkName);            // read in link name
			if (!strncmp(linkName,"root",4)) {
				indx = 0;
			} else {
				link = getLinkByName(linkName);               // find the link
				if (link == NULL){
					printf("Could not find link \'%s\'. Ignoring...\n",linkName );
					continue;
				}
				indx = link->parent->offset;
			}
			for (int n=1; n<ntokens; n++) {
				data[frameCount][indx+n-1] = float(atof(tokens[n]));
			}

			ndof += ntokens - 1;
			fgets(buffer,100,fp);                            // read next line
		}
		frameCount++;
		if (frameCount>=maxFrames-1) {
			printf("Reached maximum frame count\n");
		}
		/*
		printf("frame %5d   ndof = %3d\n",frame,ndof); 
		for (int n=0; n<ndof; n++) {
			if ((n+1)%10 == 0) printf("\n");
			printf("%5.2lf ",data[n]);
		}
		printf("\n");
		*/
		if (feof(fp)) break;
	}

	fclose(fp);
	printf("Reading %15s:  %6d frames;  frame buffer: %6d --%6d\n",fname, frameCount - startCount, startCount, frameCount-1);
	postProcess(startCount,frameCount);       // post process these frames for facing direction
}

/////////////////////////////////////////////////////////////////////////////////
// post-process motion data to add channels that describe relative movement
/////////////////////////////////////////////////////////////////////////////////

void ArticulatedFigure::postProcess(int startCount, int frameCount)
{
	Matrix4x4 toWorld;
	double values[16];
	double dx,dz;
	double th, dth;
	bool debug = false;
	bool debug2 = false;

	  // post-process data to create facing-channel directions
//	printf("Post-processing data to compute facing direction\n");
	if (debug) {       // populate data with fixed pose that is moving in a circle
		for (int f=startCount; f<frameCount; f++) {    // for each frame
			  // copy all DOFs from first frame
			for (int d=0; d<totalDOFs; d++)
				data[f][d] = data[startCount][d];
			  // fix the position
			th = f*0.02;
			data[f][0] = float(25.0*cos(th));
			data[f][2] = float(25.0*sin(th));
			  // fix the facing direction
			data[f][3] = 0;
			data[f][4] = float(-th*180.0/3.1415626);
			data[f][5] = 0;
		}
	}
	for (int f=startCount; f<frameCount; f++) {    // for each frame
		playBack(f);
		QRoot.getRotationMatrix(&toWorld);       // convert root quaternion (QRoot) to a rotation matrix (toWorld)
		toWorld.getOGLValues(values);            // convert rotation matrix (toWorld) to an array for OpenGL

		  // compute and store facing angle in XZ plane
		dx = values[8];
		dz = values[10];
		th = 180.0/3.1416*atan2(dz,dx);
		data[f][totalDOFs+0] = (float) th;

		  // compute and store dth, dx, dz
		if (f==startCount) {
			dth = dx = dz = 0.0;
		} else {
			dth = data[f][totalDOFs+0] - data[f-1][totalDOFs+0];
			dx = data[f][0] - data[f-1][0];
			dz = data[f][2] - data[f-1][2];
		}
		data[f][totalDOFs+1] = (float) dth;
		data[f][totalDOFs+2] = (float) dx;
		data[f][totalDOFs+3] = (float) dz;
	}
	if (debug2) {
		for (int f=startCount; f<startCount+20; f++) {
			printf("abs %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f rel %6.2f %6.3f %6.3f %6.3f\n",
				data[f][0],data[f][1],data[f][2],data[f][3],data[f][4],data[f][5], 
				data[f][totalDOFs+0],data[f][totalDOFs+1],data[f][totalDOFs+2],data[f][totalDOFs+3]);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//	upper body swap
////////////////////////////////////////////////////////////////////////////////////////

void ArticulatedFigure::splice(int f1, int f2, int nframes) {
	for (int f=0; f<nframes; f++) {
		for (int n=23; n<totalDOFs; n++) {
			float tmp = data[f1+f][n];
			data[f1+f][n] = data[f2+f][n];
			data[f2+f][n] = tmp;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//	playback of a blend of two frames
////////////////////////////////////////////////////////////////////////////////////////

void ArticulatedFigure::playBack2(int f1, int f2, double w1) {
	float frameData[maxChannels];

//	playBack(data[f1]);
//	return;
	if (w1==1.0) {
		playBack(data[f1]);
	} else if (w1==0.0) {
		playBack(data[f2]);
	} else {
		playBack(data[f1]);
		return;
		double w2 = 1.0 - w1;
		for (int n=0; n<6; n++)
			frameData[n] = data[f1][n];
		for (int n=6; n<=totalDOFs+3; n++) {
			frameData[n] = w1*data[f1][n] + w2*data[f2][n];
		}
		playBack(frameData);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//	playback of a given frame
////////////////////////////////////////////////////////////////////////////////////////

void ArticulatedFigure::playBack(int f) {
	playBack(data[f]);
}

void ArticulatedFigure::playBack(float *frameData) {
	  // root link DOFs
    Vector3d tmpAxis = Vector3d(1,0,0);
	QRoot = Quaternion::getRotationQuaternion(0.0,tmpAxis);
	PRoot = Vector3d();
	for (unsigned int i=0;i<rootDofs.size();i++) {
		double v = frameData[i];
		if (rootDofs[i]->rot == true)
			QRoot = Quaternion::getRotationQuaternion(RAD(v), rootDofs[i]->axis) * QRoot;
		else
			PRoot += rootDofs[i]->axis * v;
	}
	  // remaining DOFs
	for (int n=0; n<int(links.size()); n++) {
		Link *tLink = links[n];
		int indx = tLink->parent->offset;
		tLink->parent->qRel = Quaternion::getRotationQuaternion(0.0,tmpAxis);
		for (unsigned int i=0;i<tLink->parent->dofs.size();i++){
			double v = frameData[indx+i];
			tLink->parent->qRel = Quaternion::getRotationQuaternion(RAD(v), tLink->parent->dofs[i]->axis) * 
				tLink->parent->qRel;
		}
	}    // end loop over links

	  // integration of global position and the facing direction
	double th  = frameData[totalDOFs+0];
	double dth = frameData[totalDOFs+1];
	double dx  = frameData[totalDOFs+2];
	double dz  = frameData[totalDOFs+3];
	double alpha = (Cth - th)*3.1416/180.0;
	Wth = th;
	Cth += dth;
	Cx  += dx*cos(alpha) - dz*sin(alpha);
	Cz  += dx*sin(alpha) + dz*cos(alpha);
}

////////////////////////////////////////////////////////////////////////////////////////
//	Read one frame from an AMC file
////////////////////////////////////////////////////////////////////////////////////////

void ArticulatedFigure::readAMCFrame(FILE* fp){
	long oldPos;
	char buffer[100];
	bool begin = false;

	if (fp == NULL) return;           // quit if there is no valid file pointer
	while(!feof(fp)){                 // while we have not reached the end of the file
		oldPos = ftell(fp);           // record current position in the file
		fgets(buffer, 100, fp);       // read in a line
		int tempFNr = 0;
		if (!begin){                  // if not at the beginning of a frame
			int rv = sscanf(buffer, "%d", &tempFNr);      // try to read an integer from buffer
			if (rv== 1) begin = true;                     // if successful, we must be at the beginning
			continue;
		} 
		if (sscanf(buffer, "%d", &tempFNr) == 1){
			fseek(fp, oldPos, SEEK_SET);         // rewind if we can see the number that indicates the next frame
			break;
		}
		//it means we already saw the frame number, so we can go ahead and read the dof values.
		std::vector<char*> tokens = getTokens(buffer);
		if (tokens.size()<=1)               // skip blank lines
			continue;
		char linkName[100];
		sscanf(tokens[0], "%s", linkName);            // read in link name
		std::vector<double> vals;
		for (unsigned int i=1;i<tokens.size();i++){   // read in link DOF values
			double temp = 0;
			sscanf(tokens[i], "%lf", &temp);
			vals.push_back(temp);
		}

		Link* tLink = this->getLinkByName(linkName);  // find the link
		if (tLink == NULL){
			printf("Could not find link \'%s\'. Ignoring...\n", linkName);
			continue;
		}

			//compute the rel joint orientations/root information, given the values for each dof channel.
	    Vector3d tmpAxis = Vector3d(1,0,0);
		if (tLink != root){
			tLink->parent->qRel = Quaternion::getRotationQuaternion(0.0,tmpAxis);
			for (unsigned int i=0;i<tLink->parent->dofs.size();i++){
				tLink->parent->qRel = Quaternion::getRotationQuaternion(RAD(vals[i]), tLink->parent->dofs[i]->axis) * tLink->parent->qRel;
			}
		} else {
			QRoot = Quaternion::getRotationQuaternion(0.0,tmpAxis);    // identity quaternion
			PRoot = Vector3d();                                        // root position = (0,0,0)
			for (unsigned int i=0;i<rootDofs.size();i++)
				if (rootDofs[i]->rot == true)
					QRoot = Quaternion::getRotationQuaternion(RAD(vals[i]), rootDofs[i]->axis) * QRoot;
				else
					PRoot += rootDofs[i]->axis * vals[i];
		}
	}   // end while
}

/////////////////////////////////////////////////////////////////
// search for link with given name
/////////////////////////////////////////////////////////////////

Link* ArticulatedFigure::getLinkByName(char* lName){
	if (strcmp(root->name, lName) == 0)
		return root;
	for (unsigned int i=0;i<links.size();i++)
		if (strcmp(links[i]->name, lName) == 0)
			return links[i];
	return NULL;
}

/////////////////////////////////////////////////////////////////
// prints the skelton description
/////////////////////////////////////////////////////////////////

void ArticulatedFigure::print(char* fname){
	FILE *fp = fopen(fname,"w");
	fprintf(fp,"%d links %d DOFs\n",links.size(), totalDOFs);
	for (int n=0; n<int(links.size()); n++) {
		Link *link = links[n];
		fprintf(fp,"LINK:  %12s  dofs=%2d  offset=%2d\n",link->name, link->parent->ndofs, link->parent->offset);
	}
	fclose(fp);
}

/////////////////////////////////////////////////////////////////
// loads a skeleton description
/////////////////////////////////////////////////////////////////

void ArticulatedFigure::loadASFCharacter(char* ASFFName){
	if (ASFFName == NULL){
		printf("Error: File name is NULL!");
		exit(0);
	}
	FILE* fp;
	fp = fopen(ASFFName, "rb");
	if (!fp){
		printf("Unable to open input file \'%s\'\n!", ASFFName);
		exit(0);
	}
	char buffer[300];
	std::vector<ASFBoneInfo*> tempBones;   // temporary list of bones
	printf("Reading %s\n",ASFFName);

	while (!feof(fp)){
		fgets(buffer, 300, fp);
		if (feof(fp))
			break;
		char* tempLine = lTrim(buffer);
//		printf("buffer:  %s",tempLine);

		if (tempLine[0]!=':')     // major parts start with a ":"
			continue;
	
		if (strncmp(tempLine, ":root", strlen(":root")) == 0) {
//			printf("Reading :root \n");
			readRootInformation(fp);
		} else if (strncmp(tempLine, ":bonedata", strlen(":bonedata")) == 0) {
//			printf("Reading :bonedata \n");
			readBoneDataInformation(fp, &tempBones);
		} else if (strncmp(tempLine, ":hierarchy", strlen(":hierarchy")) == 0) {
//			printf("Reading :hierarchy \n");
			readHierarchyInformation(fp, &tempBones);
		}
	}
	for (unsigned int i=0;i<tempBones.size();i++)
		delete tempBones[i];             // free up temporary bones
	print("AFout.txt");                  // print AF data to this text file
}

///////////////////////////////////////////////////////////////////////////
// read skeleton root information from ASF skeleton file
///////////////////////////////////////////////////////////////////////////

void ArticulatedFigure::readRootInformation(FILE* fp){
	char buffer[100];

	while (!feof(fp)){
		long lastLine = ftell(fp);    		// make sure we can come back here
		fgets(buffer, 100, fp);             // read a line
		char* tempLine = lTrim(buffer);     // remove white space
		if (tempLine[0]==':'){              // look for lines beginning with ":", which marks next link, and the end
//			printf("rewinding file:  %s",tempLine);
			fseek(fp, lastLine, SEEK_SET);     // rewind file
			return;                            // we're done
		}
		if (strncmp(tempLine, "order", strlen("order"))==0){    // look for lines beginning with 'order'
			tempLine = lTrim(tempLine + strlen("order"));
			rootDofs = readDOFs(tempLine);
		}
	}
}

///////////////////////////////////////////////////////////////////////////
// read skeleton hierarchy information from ASF skeleton file;
// Note that the list of bone names should already have been loaded
///////////////////////////////////////////////////////////////////////////

void ArticulatedFigure::readHierarchyInformation(FILE* fp, std::vector<ASFBoneInfo*>* tempBones){
	//at this point we must have all the bone information - we'll start out by creating all the links(bones)
	//first we need to create the root link
	root = new Link();
	root->setName("root");
	root->setID(0);

	totalDOFs = 6;    // root DOFs
	for (unsigned int i=0;i<tempBones->size();i++){
		//create a joint-link pair for each bone in the skeleton - but we'll only keep a reference to the link, but it's child joint will point to the created joint.

		//first we'll create the link - and populate the required information
		links.push_back(new Link());
		links.back()->setName((*tempBones)[i]->name);
		links.back()->setID((*tempBones)[i]->id);
		links.back()->linkAxis = (*tempBones)[i]->direction * (*tempBones)[i]->length;

		//now we'll create the joint - and connect the two - the joint is the parent of the link
		Joint *tempJoint = new Joint();
		tempJoint->setChild(links.back());

		/** 
			Transforming each of the rotation axis by C, and then not taking C into account anymore 
			is equivalent to not rotating them, 
			and performing the rotations as C*R*C^(-1)!!
		**/
		for (unsigned int j=0;j<(*tempBones)[i]->dofs.size();j++){
			(*tempBones)[i]->dofs[j]->axis = (*tempBones)[i]->C * (*tempBones)[i]->dofs[j]->axis;
			tempJoint->dofs.push_back(new DOFInfo(*(*tempBones)[i]->dofs[j]));
		}
		int jointDOFs = int(tempJoint->dofs.size());
		tempJoint->ndofs = jointDOFs;
		tempJoint->offset = totalDOFs;
		totalDOFs += jointDOFs;
	}
	
	//now that we've created all the links, we'll read in the hierarchy information and link them all up
	char buffer[100];
	while(!feof(fp)){
		fgets(buffer, 100, fp);
		if (strncmp(lTrim(buffer), "end", strlen("end")) == 0)
			break;
		std::vector<char*> tokens = getTokens(buffer);
		if (tokens.size() <= 1)
			continue;

		//we now need to find the first link - this is the link that will be the parent of all the other ones in the list
		char parent[100];
		sscanf(tokens[0], "%s", parent);
		Link* pLink = getLinkByName(parent);
		if (pLink == NULL)
			continue;
		//now get all the child link names, find them and link them
		for (unsigned int i=1;i<tokens.size();i++){
			char child[100];
			sscanf(tokens[i], "%s", child);
			Link* cLink = getLinkByName(child);
			if (cLink == NULL)
				continue;
			//now that we know pLink needs to be cLink's parent, we link them
			cLink->getParent()->setParent(pLink);
		}
	}

	// count total DOFS
	int count=0;
	for (unsigned int i=0; i<links.size();i++){
		count += int(links[i]->parent->dofs.size());
	}
//	printf("Total of %d DOFs\n",count);

	//when all this is done, we will check and see if we've used up all the links that we created
	for (unsigned int i=0;i<links.size();i++){
		if (links[i]->getParent()!= NULL && links[i]->getParent()->getParent() == NULL)
			printf("Warning: Link \'%s\' was not used - this will result in memory leaks!\n" , links[i]->getName());
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//	Extract bone names from ASF file
////////////////////////////////////////////////////////////////////////////////////////

void ArticulatedFigure::readBoneDataInformation(FILE* fp, std::vector<ASFBoneInfo*>* tempBones){
	char buffer[100];
	int count=0;

	while (!feof(fp)){
		long lastLine = ftell(fp);         // make sure we can return to this point
		fgets(buffer, 100, fp);            // read a line
		char* tempLine = lTrim(buffer);    // remove white space, i.e., spaces and tabs
		if (tempLine[0]==':'){             // look for lines that begin with ":"
			fseek(fp, lastLine, SEEK_SET);
			return;
		}
		//we only care about the line that starts with 'begin'
		if (strncmp(tempLine, "begin", strlen("begin"))==0){
			tempBones->push_back(new ASFBoneInfo(fp));	    // read new bone info and add to bone list
			count++;
		}
	}
	printf("skeleton:  %d bones\n",count);
}

////////////////////////////////////////////////////////////////////////////////////////
//	constructor that populates bone info
////////////////////////////////////////////////////////////////////////////////////////

ASFBoneInfo::ASFBoneInfo(FILE* fp){
	char buffer[100];

	while(!feof(fp)){    // stop at end-of-file
		fgets(buffer, 100, fp);
		char* tempLine = lTrim(buffer);
		
		if (!strncmp(tempLine, "end", strlen("end")))   // check for end of bone-info section
			return;

		if (!strncmp(tempLine, "id", strlen("id"))){    // bone ID
			//if it is, read it in
			tempLine = lTrim(tempLine + strlen("id"));
			sscanf(tempLine, "%d", &id);
			continue;
		}

		//if it's the name, read it in
		if (!strncmp(tempLine, "name", strlen("name"))){
			//if it is, read it in
			tempLine = lTrim(tempLine + strlen("name"));
			sscanf(tempLine, "%s", name, 100);
			continue;
		}

		//see if it is the direction
		if (!strncmp(tempLine, "direction", strlen("direction"))){
			//if it is, read the components in
			tempLine = lTrim(tempLine + strlen("direction"));
			sscanf(tempLine, "%lf %lf %lf", &direction.x, &direction.y, &direction.z);
			continue;
		}

		//see if it is the length
		if (!strncmp(tempLine, "length", strlen("length"))){
			//if it is, read the components in
			tempLine = lTrim(tempLine + strlen("length"));
			sscanf(tempLine, "%lf", &length);
			continue;
		}

		//see if it is the axis
		if (!strncmp(tempLine, "axis", strlen("axis"))){
			Vector3d temp;
			//if it is, read the components in
			tempLine = lTrim(tempLine + strlen("axis"));
			//NOTE: I'll assume for now that all axis are specified in the XYZ order!!!
			sscanf(tempLine, "%lf %lf %lf", &temp.x, &temp.y, &temp.z);
			//now we'll create the C matrix
			C.loadIdentity();
         Vector3d x(1,0,0);
         Vector3d y(0,1,0);
         Vector3d z(0,0,1);
            
         Matrix4x4 tempMatrix1, tempMatrix2, tempMatrix3, tempMatrix4;
			tempMatrix1.setToRotationMatrix(RAD(temp.z), z);
			tempMatrix2.setToRotationMatrix(RAD(temp.y), y);
			tempMatrix3.setToRotationMatrix(RAD(temp.x), x);
			tempMatrix4.setToProductOf(tempMatrix1, tempMatrix2);
			C.setToProductOf(tempMatrix4, tempMatrix3);
			continue;
		}

		//finally we'll need to read in the dof's
		if (!strncmp(tempLine, "dof", strlen("dof"))){
			this->dofs = readDOFs(lTrim(tempLine + strlen("dof")));
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//	reads DOF info from an ASF-style string and returns a DynamicArray of pointers to DOFInfo structs
////////////////////////////////////////////////////////////////////////////////////////

std::vector<DOFInfo*> readDOFs(char* tempLine){
	std::vector<DOFInfo*> dofs;
	while (tempLine[0]!='\0'){
		bool foundDOF = false;
		bool rot = false;
		if (tempLine[0] == 'r' || tempLine[0] == 'R'){
			foundDOF = true;
			rot = true;
		}
		else if (tempLine[0] == 't' || tempLine[0] == 'T'){
			foundDOF = true;
			rot = false;
		}
		if (foundDOF == true){                 // if we found DOF, we need to find its axis
			Vector3d axis = Vector3d(1,0,0);
			if (tempLine[1] == 'y' || tempLine[1] == 'Y')
				axis = Vector3d(0,1,0);
			if (tempLine[1] == 'z' || tempLine[1] == 'Z')
				axis = Vector3d(0,0,1);
			dofs.push_back(new DOFInfo(axis, rot));  // add the dof to the root of the articulated figure
		}
		//we don't support the shortening dof here - for now at least 
		// (wouldn't be hard - translation dof along the direction axis)

		//the two here indicates that each DOF of the root is represented by 2 chars
		tempLine = lTrim(tempLine + 2);
	}
	return dofs;
}
