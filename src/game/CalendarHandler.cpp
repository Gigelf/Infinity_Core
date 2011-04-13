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

#include "Common.h"
#include "Log.h"
#include "Player.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "World.h"
#include "MapPersistentStateMgr.h"
#include "CalendarMgr.h"

void WorldSession::HandleCalendarGetCalendar(WorldPacket &/*recv_data*/)
{
    DEBUG_LOG("WORLD: CMSG_CALENDAR_GET_CALENDAR");

    time_t cur_time = time(NULL);

    WorldPacket data(SMSG_CALENDAR_SEND_CALENDAR, 4+4*0+4+4*0+4+4);

	sCalendarMgr->AppendInvitesToCalendarPacketForPlayer(data, GetPlayer());
	sCalendarMgr->AppendEventsToCalendarPacketForPlayer(data, GetPlayer());

    data << uint32(cur_time);                               // current time, unix timestamp
    data << (uint32) secsToTimeBitFields(cur_time);         // current packed time

    uint32 counter = 0;
    size_t p_counter = data.wpos();
    data << uint32(counter);                                // instance state count

    for(uint8 i = 0; i < MAX_DIFFICULTY; ++i)
    {
        for (Player::BoundInstancesMap::const_iterator itr = _player->m_boundInstances[i].begin(); itr != _player->m_boundInstances[i].end(); ++itr)
        {
            if(itr->second.perm)
            {
                DungeonPersistentState *state = itr->second.state;
                data << uint32(state->GetMapId());
                data << uint32(state->GetDifficulty());
                data << uint32(state->GetResetTime() - cur_time);
                data << ObjectGuid(state->GetInstanceGuid());
                ++counter;
            }
        }
    }
    data.put<uint32>(p_counter,counter);

    data << uint32(INSTANCE_RESET_SCHEDULE_START_TIME + sWorld.getConfig(CONFIG_UINT32_INSTANCE_RESET_TIME_HOUR) * HOUR);
    counter = 0;
    p_counter = data.wpos();
    data << uint32(counter);                                // Instance reset intervals
    for(MapDifficultyMap::const_iterator itr = sMapDifficultyMap.begin(); itr != sMapDifficultyMap.end(); ++itr)
    {
        MapDifficultyEntry const* mapDiff = itr->second;

        if(!mapDiff || mapDiff->resetTime == 0)
            continue;

        const MapEntry* map = sMapStore.LookupEntry(mapDiff->MapId);
        if(!map || !map->IsRaid())
            continue;

        uint32 period =  uint32(mapDiff->resetTime / DAY * sWorld.getConfig(CONFIG_FLOAT_RATE_INSTANCE_RESET_TIME)) * DAY;
        if (period < DAY)
            period = DAY;

        data << uint32(mapDiff->MapId);
        data << uint32(period);
        data << uint32(mapDiff->resetTime);
        ++counter;
    }
    data.put<uint32>(p_counter,counter);
	uint32 holidaycount;

	// holiday count
	holidaycount = sHolidaysStore.GetNumRows();
	data << uint32(holidaycount);
	for (uint32 i = 1; i < holidaycount; ++i)
	{
		if (HolidaysEntry const* holiday = sHolidaysStore.LookupEntry(i))
		{
			data << holiday->ID;
			data << holiday->unk37;
			data << holiday->unk38;
			data << holiday->unk52;
			data << holiday->RepeatingMethod;

			for(uint32 j = 0; j < 26; j++)
				data << holiday->Dates[j];

			for(uint32 j = 0; j < 10; j++)
				data << holiday->unk1[j];

			for(uint32 j = 0; j < 10; j++)
				data << holiday->unk39[j];

			data << holiday->texture;
		}
	}

    sLog.outDebug("Sending calendar");
    SendPacket(&data);
}

void WorldSession::HandleCalendarGetEvent(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: CMSG_CALENDAR_GET_EVENT");
	uint64 eventId;
	recv_data >> eventId;
	if (!eventId)
		return;
	SendCalendarEvent(eventId);
}

void WorldSession::HandleCalendarGuildFilter(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: CMSG_CALENDAR_GUILD_FILTER");
	uint32 minLevel;
	uint32 maxLevel;
	uint32 minRank;
	recv_data >> minLevel;
	recv_data >> maxLevel;
	recv_data >> minRank;
}

void WorldSession::HandleCalendarArenaTeam(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: CMSG_CALENDAR_ARENA_TEAM");
    uint32 unk1;
	recv_data >> unk1;
}

void WorldSession::HandleCalendarAddEvent(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: CMSG_CALENDAR_ADD_EVENT");
	std::string title;
	std::string description;
	uint8 type;
	uint8 Repeat_Option;
	uint32 maxInvites;
	uint32 dungeonId;
	uint32 eventPackedTime;
	uint32 lockoutPackedTime;
	uint32 flags;

	recv_data >> title;
	recv_data >> description;
	recv_data >> type;
	recv_data >> Repeat_Option;
	recv_data >> maxInvites;
	recv_data >> dungeonId;
	recv_data >> eventPackedTime;
	recv_data >> lockoutPackedTime;
	recv_data >> flags;

	CalendarEvent m_event;
	m_event.id = sCalendarMgr->GetNextEventID();
	m_event.name = title;
	m_event.description = description;
	m_event.type = type;
	m_event.Repeat_Option = Repeat_Option;
	m_event.dungeonID = dungeonId;
	m_event.flags = flags;
	m_event.time = eventPackedTime;
	m_event.lockoutTime = lockoutPackedTime;
	m_event.creator_guid = GetPlayer()->GetGUID();

   sCalendarMgr->AddEvent(m_event);

	if (((flags >> 6) & 1))
		return;

	uint32 inviteCount;
	recv_data >> inviteCount;

	if (!inviteCount)
		return;

	uint64 guid;
	uint8 status;
	uint8 rank;

	for (uint32 i = 0; i < inviteCount; ++i)
	{
		CalendarInvite invite;
		invite.id = sCalendarMgr->GetNextInviteID();
		guid = recv_data.readPackGUID();
		recv_data >> status;
		recv_data >> rank;
		invite.event = m_event.id;
		invite.creator_guid = GetPlayer()->GetGUID();
		invite.target_guid = guid;
		invite.status = status;
		invite.rank = rank;
		invite.time = m_event.time;
		invite.text = ""; // hmm...
		invite.mod_Type = invite.invite_Type = invite.unk3 = 0;
		sCalendarMgr->AddInvite(invite);
	}

	SendCalendarEvent(m_event.id, true); 
}

void WorldSession::HandleCalendarUpdateEvent(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: CMSG_CALENDAR_UPDATE_EVENT");
	uint64 eventID;
	uint64 inviteID;
	std::string title;
	std::string description;
	uint8 mod_flag;
	uint8 repeat_option;
	uint32 maxInvite;
	uint32 dungeonID;
	uint32 startTime;
	uint32 endTime;
	uint32 flags;

    recv_data >> eventID;
    recv_data >> inviteID;
    recv_data >> title;
    recv_data >> description;
    recv_data >> mod_flag;
    recv_data >> repeat_option;
    recv_data >> maxInvite;
    recv_data >> dungeonID;
    recv_data >> startTime;
    recv_data >> endTime;
    recv_data >> flags;
}

void WorldSession::HandleCalendarRemoveEvent(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: CMSG_CALENDAR_REMOVE_EVENT");
	uint64 eventId;
	uint64 creatorGuid;
	uint32 unk1;

	recv_data >> eventId;
	recv_data >> creatorGuid;
	recv_data >> unk1;

	sCalendarMgr->RemoveEvent(eventId);
}

void WorldSession::HandleCalendarCopyEvent(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: CMSG_CALENDAR_COPY_EVENT");
	uint64 eventID;
	uint64 creatorGuid;
	uint32 Time;

    recv_data >> eventID;
    recv_data >> creatorGuid;
    recv_data >> Time;
}

void WorldSession::HandleCalendarEventInvite(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: CMSG_CALENDAR_EVENT_INVITE");

	uint64 eventId;
	uint64 inviteId;
	std::string name;
	uint8 status;
	uint8 rank;

	recv_data >> eventId;
	recv_data >> inviteId;
	recv_data >> name;
	recv_data >> status;
	recv_data >> rank;

	//FIXME - Finish it
}

void WorldSession::HandleCalendarEventRsvp(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: CMSG_CALENDAR_EVENT_RSVP");
	uint64 eventId;
	uint64 inviteId;
	uint32 status;

    recv_data >> eventId;
    recv_data >> inviteId;
    recv_data >> status;
}

void WorldSession::HandleCalendarEventRemoveInvite(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: CMSG_CALENDAR_EVENT_REMOVE_INVITE");
	uint64 RemoveGuid;
	uint64 RemoveInviteID;
	uint64 RemoverGuid;
	uint64 eventID;

    RemoveGuid = recv_data.readPackGUID();
    recv_data >> RemoveInviteID;
    recv_data >> RemoverGuid;
    recv_data >> eventID;
}

void WorldSession::HandleCalendarEventStatus(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: CMSG_CALENDAR_EVENT_STATUS");
	uint64 inviteeGUID;
	uint64 eventID;
	uint64 inviteeInviteID;
	uint64 inviterInviteID;
	uint32 inviteStatus;

    inviteeGUID = recv_data.readPackGUID();
    recv_data >> eventID;
    recv_data >> inviteeInviteID;
    recv_data >> inviterInviteID;
    recv_data >> inviteStatus;
}

void WorldSession::HandleCalendarEventModeratorStatus(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: CMSG_CALENDAR_EVENT_MODERATOR_STATUS");
	uint64 participant;
	uint64 eventID;
	uint64 InviteID1;
	uint64 InviteID2;
	uint32 inviteStatus;

    participant = recv_data.readPackGUID();
    recv_data >> eventID;
    recv_data >> InviteID1;
    recv_data >> InviteID2;
    recv_data >> inviteStatus;
}

void WorldSession::HandleCalendarComplain(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: CMSG_CALENDAR_COMPLAIN");
    recv_data.hexlike();
    recv_data.rpos(recv_data.wpos());                       // set to end to avoid warnings spam

    //recv_data >> uint64
    //recv_data >> uint64
    //recv_data >> uint64
}

void WorldSession::HandleCalendarGetNumPending(WorldPacket & /*recv_data*/)
{
    DEBUG_LOG("WORLD: CMSG_CALENDAR_GET_NUM_PENDING");      // empty

    WorldPacket data(SMSG_CALENDAR_SEND_NUM_PENDING, 4);
    data << uint32(0);                                      // 0 - no pending invites, 1 - some pending invites
    SendPacket(&data);
}

void WorldSession::SendCalendarEvent(uint64 eventId, bool added)
{
	sLog.outDebug("SMSG_CALENDAR_SEND_EVENT");
	WorldPacket data(SMSG_CALENDAR_SEND_EVENT);
	CalendarEvent *m_event = sCalendarMgr->GetEvent(eventId);
	data << uint8(added);											// from add_event
	data.appendPackGUID(m_event->creator_guid);						// creator GUID
	data << uint64(eventId);										// event ID
	data << m_event->name;											// event name
	data << m_event->description;									// event description
	data << uint8(m_event->type);                                   // event type
	data << uint8(m_event->Repeat_Option);                          // Repeat_Option
	data << uint32(100);											// Max invites
	data << int32(m_event->dungeonID);                              // dungeon ID
	data << uint32(m_event->flags);                                 // event flags
	data << uint32(m_event->time);                                  // event time
	data << uint32(m_event->lockoutTime);                           // LockoutTime

	if (false) // invites exist
	{
		data << uint32(0);                                  // invite count
		for (uint8 i = 0; i < 0; ++i)
		{
			data.appendPackGUID(0);                         // invite player guid
			data << uint8(0);                               // level
			data << uint8(0);                               // status
			data << uint8(0);                               // rank
			data << uint8(0);                               // unk
			data << uint64(0);                              // invite ID
			data << uint32(0);                              // last Updated
		}
	}
	SendPacket(&data);
}

void WorldSession::SendCalendarEventInviteAlert(uint64 eventId, uint64 inviteId)
{
	sLog.outDebug("SMSG_CALENDAR_EVENT_INVITE_ALERT");
	WorldPacket data(SMSG_CALENDAR_EVENT_INVITE_ALERT, 0); //no idea about size

	CalendarEvent *m_event = sCalendarMgr->GetEvent(eventId);
	CalendarInvite *invite = sCalendarMgr->GetInvite(inviteId);

	data << uint64(eventId);						// event ID
	data << m_event->name;							// event title
	data << uint32(m_event->time);					// event time
	data << uint32(m_event->flags);					// event flags
	data << uint8(m_event->type);					// event type
	data << uint32(m_event->dungeonID);				// dungeon id
	data << uint64(inviteId);						// invite id
	data << uint8(invite->status);					// invite status
	data << uint8(invite->rank);					// invite rank
	data.appendPackGUID(m_event->creator_guid);     // event creator
	data.appendPackGUID(invite->creator_guid);		// invite sender
	SendPacket(&data);
}

void WorldSession::SendCalendarEventRemovedAlert(uint64 eventId)
{
	sLog.outDebug("SMSG_CALENDAR_EVENT_REMOVED_ALERT");
	WorldPacket data(SMSG_CALENDAR_EVENT_REMOVED_ALERT);

	CalendarEvent *m_event = sCalendarMgr->GetEvent(eventId);
	data << uint8(0);                            // unk
	data << uint64(eventId);                     // event id
	data << uint32(m_event->time);               // invite time
	SendPacket(&data);
 }
