/****************************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

#include <stdio.h>
#include <media/MediaPlayer.h>
#include <media/FileInputDataSource.h>

#define MEDIAPLAYER_TEST_FILE "/ramfs/utc_media_player.raw"

class SimpleMediaPlayerTest : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		fp = fopen(MEDIAPLAYER_TEST_FILE, "w");
		fputs("dummyfile", fp);
		source = std::move(std::unique_ptr<media::stream::FileInputDataSource>(
			new media::stream::FileInputDataSource(MEDIAPLAYER_TEST_FILE)));
		source->setSampleRate(16000);
		source->setChannels(2);
		mp = new media::MediaPlayer();
	}
	virtual void TearDown()
	{
		fclose(fp);
		remove(MEDIAPLAYER_TEST_FILE);
		delete mp;
	}

	FILE* fp;
	std::unique_ptr<media::stream::FileInputDataSource> source;
	media::MediaPlayer* mp;
};

TEST_F(SimpleMediaPlayerTest, getState)
{
	EXPECT_EQ(mp->getState(), media::PLAYER_STATE_NONE);
}

TEST_F(SimpleMediaPlayerTest, create)
{
	ASSERT_EQ(mp->create(), media::PLAYER_OK);
	EXPECT_EQ(mp->getState(), media::PLAYER_STATE_IDLE);

	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, destroy)
{
	mp->create();

	ASSERT_EQ(mp->destroy(), media::PLAYER_OK);
	EXPECT_EQ(mp->getState(), media::PLAYER_STATE_NONE);
}

TEST_F(SimpleMediaPlayerTest, setDataSource)
{
	mp->create();

	ASSERT_EQ(mp->setDataSource(std::move(source)), media::PLAYER_OK);

	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, prepare)
{
	mp->create();
	mp->setDataSource(std::move(source));

	ASSERT_EQ(mp->prepare(), media::PLAYER_OK);
	EXPECT_EQ(mp->getState(), media::PLAYER_STATE_READY);

	mp->unprepare();
	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, unprepare)
{
	mp->create();
	mp->setDataSource(std::move(source));
	mp->prepare();

	EXPECT_EQ(mp->unprepare(), media::PLAYER_OK);
	EXPECT_EQ(mp->getState(), media::PLAYER_STATE_IDLE);

	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, setVolume)
{
	mp->create();
	mp->setDataSource(std::move(source));
	mp->prepare();

	for (int vol = 0; vol <= 31; vol++) {
		EXPECT_EQ(mp->setVolume(vol), media::PLAYER_OK);
	}

	mp->unprepare();
	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, getVolume)
{
	mp->create();
	mp->setDataSource(std::move(source));
	mp->prepare();

	for (int vol = 0; vol <= 31; vol++) {
		mp->setVolume(vol);
		EXPECT_EQ(mp->getVolume(), vol);
	}

	mp->unprepare();
	mp->destroy();
}
