class Session(object):

    def __init__(self, env, sid = None):
        if sid is None: 
            sid = hashlib.md5(
                    str(
                        decoder.decode(
                            env.request.headers.get('Session-Id').decode('hex')
                            )[0][4]
                        )
                    ).hexdigest()
        env.sid = sid
        object.__setattr__(self, 'sid', sid)
        object.__setattr__(self, '__changed__', False)
        object.__setattr__(self, '__driver__', env.session_storage)
        object.__setattr__(self, '__cache__', env.session_storage.get(sid) or {})
        object.__setattr__(self, '__online__', env.online)


    def authorize(self, **kw):
        self.__changed__ =  True
        self.__cache__.update(kw)
    
    def delete(self):
        print self, 'logout'
        self.__cache__['is_auth'] = False
        self.__driver__.delete(self.sid)
        if self.id in self.__online__:
            del self.__online__[self.id]

    def __repr__(self):
        return '<Session>'
    
    def __setattr__(self, key, value):
        if key.startswith("__"):
            object.__setattr__(self, key, value)
        else:
            self.__changed__ = True
            self.__cache__[key] = value
    
    def __delattr__(self, key):
        self.__changed__ = True
        del self.__cache__[key]
    
    def __getattr__(self, key):
        return self.__cache__[key] if self.__cache__.has_key(key) else None

    def flush(self):
        if self.__changed__:
            self.__driver__.set(self.sid, self.__cache__)

    def __del__(self):
        self.flush()


