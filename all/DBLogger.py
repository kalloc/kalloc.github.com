# -*- coding: utf-8 -*-
from ra import cfg
from mage import sqla
from ra.data import lang
from Queue import Queue,Empty
from ra import models as m
from twisted.internet import reactor
from time import sleep
from sqlalchemy import text
class Logger(object):
    session = sqla.construct_maker(cfg.LOG_DATABASES, models = "web.models", engine_params = {'pool_size':1})

    flag = True
    thread_id = 0

    def __init__(self):
        self.facilities = set(lang.LOG_FACILITIES.keys())
        self.levels = set(lang.LOG_LEVELS.keys())
        map(lambda name: setattr(self, name, name), lang.LOG_TITLES.keys())
        self.queue = Queue()
        reactor.callInThread(self.__run__)
        reactor.addSystemEventTrigger('before', 'shutdown', self.__stop__)

    def __getattr__(self, name):
        facility = None
        level = None
        if name.find("_") > 0:
            (facility, level) = name.split("_")
        else:
            if name in self.facilities:
                facility = name
            elif name in self.levels:
                level = name
        def hook(title, body = None,  **kwargs):
            if not hasattr(kwargs, 'facility') or not kwargs['facility']:
                kwargs['facility'] = facility
            if not hasattr(kwargs, 'level') or not kwargs['level']:

                kwargs['level'] = level
            return self.add(title, body, **kwargs)
        return hook

    def __run__(self):
        self.thread_id = self.add('SERVER_START', '', need_thread_id = True)

        try:
            while self.flag:
                try:
                    entry = self.queue.get_nowait()
                except Empty:
                    sleep(0.1)
                    continue
                self.save_to_db(**entry)
                self.queue.task_done()
        except Exception as e:
            print '!'*10,e.args

    def __stop__(self, *args, **kwargs):
        self.add('SERVER_STOP', '', need_thread_id = True)
        self.flag = False

    def __add__(self, params):
        if isinstance(params, dict):
            self.add(**params)
        else:
            self.add(*params)
        return self

    def save_to_db(self, **kwargs):
        print self, kwargs
        db = self.session()
        log = db.query(m.Log).filter_by(**kwargs).filter(m.Log.date >= text("now() - '30 minutes'::interval")).first()
        if log:
            log.count=log.count+1
        else:
            log = m.Log(**kwargs)
        db.add(log)
        try:
            db.commit()
        except Exception, e:
            print 'ERROR in DBLogger',e 
            db.rollback()
        log_id = log.id
        db.close()
        return log_id
        

    def add(self, title, body = "", need_thread_id = False, **kwargs):
        kwargs['title'] = title
        kwargs['body'] = body
        if not hasattr(kwargs, 'thread_id') or not kwargs['thread_id'] :
            kwargs['thread_id'] = self.thread_id
        if need_thread_id:
            return self.save_to_db(**kwargs)
        else:
            return self.queue.put_nowait(kwargs)

DBLogger = Logger()
add = DBLogger.add
