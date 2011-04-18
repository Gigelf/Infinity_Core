/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MANGOS_CALENDAR_H
#define MANGOS_CALENDAR_H

#include "Common.h"

enum CalendarEventType
{
	CALENDARTYPE_RAID,
	CALENDARTYPE_DUNGEON,
	CALENDARTYPE_PVP,
	CALENDARTYPE_MEETING,
	CALENDARTYPE_OTHER
};

enum CalendarInviteStatus
{
	CALENDARSTATUS_INVITED,
	CALENDARSTATUS_ACCEPTED,
	CALENDARSTATUS_DECLINED,
	CALENDARSTATUS_CONFIRMED,
	CALENDARSTATUS_OUT,
	CALENDARSTATUS_STANDBY,
	CALENDARSTATUS_SIGNEDUP,
	CALENDARSTATUS_NOT_SIGNEDUP
};

enum CalendarEventFlags
{
	CalendarEventFlags_Player = 0x001,
	CalendarEventFlags_System = 0x004,
	CalendarEventFlags_Holiday = 0x008,
	CalendarEventFlags_Locked = 0x10,
	CalendarEventFlags_AutoApprove = 0x20,
	CalendarEventFlags_GuildAnnouncement = 0x040,
	CalendarEventFlags_RaidLockout = 0x080,
	CalendarEventFlags_RaidReset = 0x200,
	CalendarEventFlags_GuildEvent = 0x400
};

enum CalendarInviteType
{
	CalendarInviteType_Normal = 0,
	CalendarInviteType_Signup = 1
};

enum CalendarModType
{
	CalendarModType_Participant = 0,
	CalendarModType_Moderator = 1,
	CalendarModType_Creator = 2
};

enum CalendarEventModFlags
{
	CalendarEventModFlags_Moderator = 0x2,
	CalendarEventModFlags_Creator = 0x4
};

enum CalendarRepeatOption
{
	CalendarRepeatOption_Never = 0,
	CalendarRepeatOption_Weekly = 1,
	CalendarRepeatOption_BiWeekly = 2,
	CalendarRepeatOption_Monthly = 3
};

struct CalendarEvent
{
   uint64 id;
   uint64 creator_guid;
   std::string name;
   std::string description;
   uint8 type;
   uint8 Repeat_Option;
   uint32 dungeonID;
   uint32 lockoutTime;
   uint32 time;
   uint32 flags;
};

struct CalendarInvite
{
   uint64 id;
   uint64 eventID;
   uint8 status;
   uint8 rank;
   uint8 mod_Type;
   uint8 invite_Type;
   uint8 unk3;
   std::string text;
   uint64 creator_guid;
   uint32 time;
   uint64 target_guid;
};

typedef UNORDERED_MAP<uint64, CalendarInvite> CalendarInviteMap;
typedef UNORDERED_MAP<uint64, CalendarEvent> CalendarEventMap;

class Calendar
{
public:
	Calendar();
	~Calendar();

	void Initialize();
	void SaveEvents();
	void SaveInvites();

private:
	void GetEvents();
	void GetInvites();
};

#endif
