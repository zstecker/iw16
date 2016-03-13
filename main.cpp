/*******************************************************************************
*                                                                              *
*   Zachary Stecker - Independent Work (Spring 2016)                           *
*   Using PrimeSense NiTE 2.0                                                  *
*                                                                              *
*******************************************************************************/

#include "NiTE.h"
#include <cmath>

#include "../Common/NiteSampleUtilities.h"

int main(int argc, char** argv)
{
	nite::HandTracker handTracker;
	nite::Status niteRc;

	niteRc = nite::NiTE::initialize();
	if (niteRc != nite::STATUS_OK)
	{
		printf("NiTE initialization failed\n");
		return 1;
	}

	niteRc = handTracker.create();
	if (niteRc != nite::STATUS_OK)
	{
		printf("Couldn't create user tracker\n");
		return 3;
	}

	handTracker.startGestureDetection(nite::GESTURE_WAVE);
	handTracker.startGestureDetection(nite::GESTURE_CLICK);
	printf("\nWave or click to start tracking your hand...\n");


	// establishing baseline here



	nite::HandTrackerFrameRef handTrackerFrame;
	while (!wasKeyboardHit())
	{
		niteRc = handTracker.readFrame(&handTrackerFrame);
		if (niteRc != nite::STATUS_OK)
		{
			printf("Get next frame failed\n");
			continue;
		}

		const nite::Array<nite::GestureData>& gestures = handTrackerFrame.getGestures();
		for (int i = 0; i < gestures.getSize(); ++i)
		{
			if (gestures[i].isComplete())
			{
				nite::HandId newId;
				handTracker.startHandTracking(gestures[i].getCurrentPosition(), &newId);
			}
		}

		const nite::Array<nite::HandData>& hands = handTrackerFrame.getHands();
		

		/*********************************
		*  DEBUGGING                     *
		**********************************/

		// -pos flag: print all hand data
		if (strcmp(argv[1], "-pos") == 0) {
			for (int i = 0; i < hands.getSize(); ++i)
			{
				const nite::HandData& hand = hands[i];
				if (hand.isTracking())
				{
					printf("%d. (%5.2f, %5.2f, %5.2f)\n", hand.getId(), hand.getPosition().x, hand.getPosition().y, hand.getPosition().z);
					// if (hand.getPosition().x > 0.0 && hand.getPosition().y > 0.0) printf("%s\n", "right / top");
					// else if (hand.getPosition().x > 0.0 && hand.getPosition().y < 0.0) printf("%s\n", "right / bottom");
					// else if (hand.getPosition().x < 0.0 && hand.getPosition().y < 0.0) printf("%s\n", "left / bottom");
					// else printf("%s\n", "left / top");
				}
			}
		}
		
		// -dist flag: if two hands are detected, print absolute distance between them
		if (strcmp(argv[1], "-dist") == 0) {
			if (hands.getSize() == 2) {
				const nite::HandData& hand0 = hands[0];
				const nite::HandData& hand1 = hands[1];
				if (hand0.isTracking() && hand1.isTracking()) {
					float xdist = std::abs ((hand0.getPosition().x) - (hand1.getPosition().x));
					printf("%s %5.2f\n", "x dist: ", xdist);
				}
			}
		}
	}

	nite::NiTE::shutdown();

}
