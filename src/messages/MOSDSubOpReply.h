// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2006 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software 
 * Foundation.  See file COPYING.
 * 
 */


#ifndef __MOSDSUBOPREPLY_H
#define __MOSDSUBOPREPLY_H

#include "msg/Message.h"

#include "MOSDSubOp.h"
#include "os/ObjectStore.h"

/*
 * OSD op reply
 *
 * oid - object id
 * op  - OSD_OP_DELETE, etc.
 *
 */

class MOSDSubOpReply : public Message {
  epoch_t map_epoch;
  
  // subop metadata
  osd_reqid_t reqid;
  pg_t pgid;
  tid_t rep_tid;
  int32_t op;
  pobject_t poid;
  off_t length, offset;
  
  // result
  bool commit;
  int32_t result;
  
  // piggybacked osd state
  eversion_t pg_complete_thru;
  osd_peer_stat_t peer_stat;

  map<string,bufferptr> attrset;

 public:
  virtual void decode_payload() {
    bufferlist::iterator p = payload.begin();
    ::decode(map_epoch, p);
    ::decode(reqid, p);
    ::decode(pgid, p);
    ::decode(rep_tid, p);
    ::decode(op, p);
    ::decode(poid, p);
    ::decode(length, p);
    ::decode(offset, p);
    ::decode(commit, p);
    ::decode(result, p);
    ::decode(pg_complete_thru, p);
    ::decode(peer_stat, p);
    ::decode(attrset, p);
  }
  virtual void encode_payload() {
    ::encode(map_epoch, payload);
    ::encode(reqid, payload);
    ::encode(pgid, payload);
    ::encode(rep_tid, payload);
    ::encode(op, payload);
    ::encode(poid, payload);
    ::encode(length, payload);
    ::encode(offset, payload);
    ::encode(commit, payload);
    ::encode(result, payload);
    ::encode(pg_complete_thru, payload);
    ::encode(peer_stat, payload);
    ::encode(attrset, payload);
    env.data_off = offset;
  }

  epoch_t get_map_epoch() { return map_epoch; }

  pg_t get_pg() { return pgid; }
  tid_t get_rep_tid() { return rep_tid; }
  int get_op()  { return op; }
  pobject_t get_poid() { return poid; }
  const off_t get_length() { return length; }
  const off_t get_offset() { return offset; }

  bool get_commit() { return commit; }
  int get_result() { return result; }

  void set_pg_complete_thru(eversion_t v) { pg_complete_thru = v; }
  eversion_t get_pg_complete_thru() { return pg_complete_thru; }

  void set_peer_stat(const osd_peer_stat_t& stat) { peer_stat = stat; }
  const osd_peer_stat_t& get_peer_stat() { return peer_stat; }

  void set_attrset(map<string,bufferptr> &as) { attrset = as; }
  map<string,bufferptr>& get_attrset() { return attrset; } 

public:
  MOSDSubOpReply(MOSDSubOp *req, int result_, epoch_t e, bool commit_) :
    Message(MSG_OSD_SUBOPREPLY),
    map_epoch(e),
    reqid(req->get_reqid()),
    pgid(req->get_pg()),
    rep_tid(req->get_rep_tid()),
    op(req->get_op()),
    poid(req->get_poid()),
    length(req->get_length()),
    offset(req->get_offset()),
    commit(commit_),
    result(result_) {
    memset(&peer_stat, 0, sizeof(peer_stat));
  }
  MOSDSubOpReply() {}

  const char *get_type_name() { return "osd_op_reply"; }
  
  void print(ostream& out) {
    out << "osd_sub_op_reply(" << reqid
	<< " " << MOSDOp::get_opname(op)
	<< " " << poid;
    if (length) out << " " << offset << "~" << length;
    if (op >= 10) {
      if (commit)
	out << " commit";
      else
	out << " ack";
    }
    out << " = " << result;
    out << ")";
  }

};


#endif
