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

#include "Calendar.h"
#include "CalendarMgr.h"
#include "ProgressBar.h"

void Calendar::Initialize()
{
	sLog.outString("Loading Calendar events");
	GetEvents();

	sLog.outString("Loading Calendar invites");
	GetInvites();
}

void Calendar::GetEvents()
{
	//Load calendar events from DB
	QueryResult *result =  CharacterDatabase.PQuery("SELECT * FROM calendar_events");
	uint32 count = 0;
	barGoLink bar((int)result->GetRowCount());

	if (result)
	{
		do
		{
			bar.step();
			CalendarEvent m_event;
			Field *fields = result->Fetch();
			m_event.id = fields[0].GetUInt64();
			m_event.creator_guid = fields[1].GetUInt64();
			m_event.name = fields[2].GetCppString();
			m_event.description = fields[3].GetCppString();
			m_event.type = fields[4].GetUInt8();
			m_event.Repeat_Option = fields[5].GetUInt8();
			m_event.dungeonID = fields[6].GetInt32();
			m_event.lockoutTime = fields[7].GetInt32();
			m_event.time = fields[8].GetInt32();
			m_event.flags = fields[9].GetInt32();
			sCalendarMgr->AddEvent(m_event);
			count++;
		} while (result->NextRow());

		delete result;
	}

	sLog.outString("Loaded %u events", count);
}

void Calendar::GetInvites()
{
	//Load calendar invites from DB
	QueryResult *result =  CharacterDatabase.PQuery("SELECT * FROM calendar_invites");
	uint32 count = 0;
	barGoLink bar((int)result->GetRowCount());

	if (result)
	{
		do
		{
			bar.step();
			CalendarInvite m_invite;
			Field *fields = result->Fetch();
			m_invite.id = fields[0].GetUInt64();
			m_invite.eventID = fields[1].GetUInt64();
			m_invite.status = fields[2].GetUInt8();
			m_invite.rank = fields[3].GetUInt8();
			m_invite.mod_Type = fields[4].GetUInt8();
			m_invite.invite_Type = fields[5].GetUInt8();
			m_invite.unk3 = fields[6].GetUInt8();
			m_invite.text = fields[7].GetCppString();
			m_invite.creator_guid = fields[8].GetUInt64();
			m_invite.time = fields[9].GetInt32();
			m_invite.target_guid = fields[10].GetUInt64();
			sCalendarMgr->AddInvite(m_invite);
			count++;
		} while (result->NextRow());

		delete result;
	}

	sLog.outString("Loaded %u invites", count);
}

void Calendar::SaveEvents()
{
	CharacterDatabase.Execute("TRUNCATE calendar_events");

	CalendarEventMap _eventMap;
	for (CalendarEventMap::iterator itr = _eventMap.begin(); itr != _eventMap.end(); ++itr)
	{
		CalendarEvent m_event = itr->second;
		CharacterDatabase.PExecute("INSERT INTO calendar_events VALUES ('%u','%u','%s','%s','%u','%u','%u','%u','%u','%u')",
			m_event.id, m_event.creator_guid, m_event.name.c_str(), m_event.description.c_str(), m_event.type, m_event.Repeat_Option,
			m_event.dungeonID, m_event.lockoutTime, m_event.time, m_event.flags);
	}
}

void Calendar::SaveInvites()
{
	CharacterDatabase.Execute("TRUNCATE calendar_invites");

	CalendarInviteMap _inviteMap;
	for (CalendarInviteMap::iterator itr = _inviteMap.begin(); itr != _inviteMap.end(); ++itr)
	{
		CalendarInvite invite = itr->second;
		CharacterDatabase.PExecute("INSERT INTO calendar_invites VALUES ('%u','%u','%u','%u','%u','%u','%u','%s','%u','%u','%u')",
			invite.id, invite.eventID, invite.status, invite.rank, invite.mod_Type, invite.invite_Type, invite.unk3, invite.text.c_str(),
			invite.creator_guid, invite.time, invite.target_guid);
	}
}