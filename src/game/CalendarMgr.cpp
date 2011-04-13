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

#include "CalendarMgr.h"
#include "Database/QueryResult.h"

CalendarMgr::CalendarMgr() : _currentEventID(0), _currentInviteID(0)
{
}

CalendarMgr::~CalendarMgr()
{
}

void CalendarMgr::AppendInvitesToCalendarPacketForPlayer(WorldPacket &data, Player *pPlayer)
{
   size_t p_counter = data.wpos();
   uint32 counter = 0;
   data << uint32(counter);
   for (CalendarInviteMap::iterator itr = _inviteMap.begin(); itr != _inviteMap.end(); ++itr)
   {
       CalendarInvite invite = itr->second;
	   if (invite.target_guid == pPlayer->GetGUID())
       {
           data << uint64(invite.event);                // Event ID
           data << uint64(invite.id);					// Invite ID
           data << uint8(invite.status);				// status
		   data << uint8(invite.mod_Type);				// Mod Type
		   data << uint8(invite.invite_Type);			// invite_Type
           data.appendPackGUID(invite.creator_guid);	// creator's guid
           counter++;
       }
   }
   data.put<uint32>(p_counter, counter);             // update number of invites
}

void CalendarMgr::AppendEventsToCalendarPacketForPlayer(WorldPacket &data, Player *pPlayer)
{
   // TODO: There's gotta be a better way to do this
   size_t p_counter = data.wpos();
   uint32 counter = 0;
   data << uint32(counter);
   std::set<uint64> alreadyAdded;
   for (CalendarInviteMap::iterator itr = _inviteMap.begin(); itr != _inviteMap.end(); ++itr)
   {
       CalendarInvite invite = itr->second;
	   if (invite.target_guid == pPlayer->GetGUID())
       {
           if (alreadyAdded.find(invite.id) == alreadyAdded.end())
           {
               CalendarEvent *m_event = GetEvent(invite.id);
               data << uint64(m_event->id);                // event ID
               data << m_event->name;                      // event title
               data << uint32(m_event->type);              // event type
               data << uint32(m_event->time);              // event time as time bit field
               data << uint32(m_event->flags);             // event flags
               data << uint32(m_event->dungeonID);         // dungeon ID
               data.appendPackGUID(m_event->creator_guid); // creator guid
               alreadyAdded.insert(invite.id);
               counter++;
           }
       }
   }
   data.put<uint32>(p_counter, counter);             // update number of invites
}