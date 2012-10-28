var sys = require("sys"), http = require("http"), fs = require('fs'), events = require("events"), multipart  = require("multipart");
var Session = require("session").session ,Crypt = require("session").crypt,Hash = require("session").hash;
_RegExp=[]
_Static={}

 var displayError = function (e) {
        var error = '<h1>Error</h1>\n'+
            '<h2>'+e.name+'</h2>\n'+
            '<hr/>\n'+
            '<b>'+e.message+'</b>\n'    ;
        return function(req,res,match) { 
            res.simpleError(500,error) 
        }
 }

///////////////////////////////////////////////////////
//
//			Reloader
//
///////////////////////////////////////////////////////
var Reloader = function(controller) {
    this.controller=controller;
	fs.readFile(controller.file,function(err,str) {
			try {	
				controller.callback=eval(str);
			} catch (e)  {
                controller.callback=displayError(e);
			}
	});
	fs.watchFile(controller.file, function (file) {
	    fs.readFile(controller.file,function(err,str) {
			try {	
                controller.callback=eval(str);
			} catch (e)  {
                controller.callback=displayError(e);
			}
	    });
	});
}
///////////////////////////////////////////////////////
//
//			Controller
//
///////////////////////////////////////////////////////

var Controller = function (callback) {

	this.pattern =  null;
	this.count = null;
	this.method = null;
	this.static = null;
	this.file = null;
	this.reloader = null;
	this.resource = null;
	this.callback =null;

	switch (typeof(callback)) {
		case "function":
			this.setCallback(callback);
			break;
		case "string":
            this.file=callback;
			this.setCallbackFromFile(this);
			break;
		default:
			throw new Error("неверный тип callback — "+typeof(callback));
	}
};

Controller.prototype={
	setCallback : function(cb) {
		this.callback=cb;
	},
	setCallbackFromFile : function(controller) {

		this.reloader = new Reloader(controller);
	}
}



///////////////////////////////////////////////////////
//
//			Route
//
///////////////////////////////////////////////////////

var Route = function () {};

var MethodMap={
    "GET":1,
    "POST":2,
    "DELETE":4,
    "HEAD":8,
    "PUT":16
};


Route.prototype.mount = function(method,pattern,callback) {
	var controller = typeof(callback) == 'object' && callback || new Controller(callback);
    if (typeof(method) == 'string') {
        controller.method=MethodMap[method.toUpperCase()];
    } else {
        controller.method=method.reduce(function(x1,x2) {return MethodMap[x1.toUpperCase()]|MethodMap[x2.toUpperCase()]});
    }
	var isStatic=function(str) {
		if(typeof(str) != 'string'  ||  /:(number|string)/.test(str)) {
			return false;
		}
		return true;
	}
	
	var toRegExp = function(str) {
		if(typeof(str) != 'string')  return str;
		return new RegExp('^'+str.replace(/\./g,'\.').replace('/','\/').replace(/:number/g,'([0-9]+)').replace(/:string/g,'([^/]+)')+'$','g');
	}
	if(isStatic(pattern)) {
        if(!_Static[pattern]) {
            _Static[pattern]=new Array();
        }
        controller.static=pattern;
        _Static[pattern].push(controller);
	} else {
		controller.pattern=toRegExp(pattern);
		sys.puts(controller.pattern);
		_RegExp.push(controller)
	}
};

var callController = function() {
    events.EventEmitter.call(this);
} 

sys.inherits(callController, events.EventEmitter);

callController.prototype.parsePOST = function (call,req,controller,match) {
			req.setBodyEncoding("binary");
			var stream = new multipart.parse(req);
			req.POST={};
			req.FILES={};
			stream.addListener("partBegin",function(part) {
				if(part.filename) {
					if(ss.cfg.upload) {
						req.pause();
						part.tmp_filename = ss.cfg.upload+Hash.hash(req.connection.remoteAddress+part.name+part.filename)+'.tmp';
						part.size = 0;
						part.is_file = 1;
						req.FILES[part.name]={"local":part.tmp_filename,"real":part.filename,"size":0,"mime":part.headers["content-type"]};
						fs.open(part.tmp_filename,'w',0600,function(err,fd) {
							if(err) {
								call.emit('internal',err);
							} else {
								part.fd = fd;
							}
							
							if(stream.part.temp) {
								fs.write(fd,stream.part.temp,null,'binary',function() {
									stream.part.size=stream.part.temp.length;
									stream.part.temp=null;
									req.resume();
								});
							} else {
								req.resume();
							}
						});
					} else {
						call.emit('internal','upload not set');
					}
					
				} else {
					req.POST[part.name]="";
				}
			}).addListener("body",function(chunk) {
				if(stream.part.filename) {
					if(!stream.part.fd) {
						stream.part.temp=(stream.part.temp?stream.part.temp:"")+chunk;
					} else {
						req.pause();
						stream.part.size+=chunk.length;
						fs.write(stream.part.fd,chunk,null,'binary',function() {
							req.resume();
						});
					}
				} else {
					req.POST[stream.part.name]+=chunk;
				}
				

			}).addListener("partEnd",function(part) {
				if(part.fd) {
					req.pause();
					req.FILES[part.name].size=part.size;
					fs.close(part.fd,function() {
						req.resume();
					});
				}
			}).addListener("complete",function(part) {
				call.emit('found',controller,match);
				
			}).addListener("error",function(err) {
				call.emit('internal',err);
			});
		}



callController.prototype.perform=function(req) {
    var controller = this.findController(req);
    if(!controller) {
	this.emit('notfound');
    } else {
	var match = null;
	if(controller.pattern) {
		controller.pattern.lastIndex=0;
		match=controller.pattern.exec(req.url).slice(1);
	}
	if(req.method == "POST") {
		this.emit('post',controller,match);
	} else {
		this.emit('found',controller,match);
	}
    }
}
callController.prototype.findController = function (req) {
        method=MethodMap[req.method];
        var checkMethod = function(controllers,method) {
            for(var key in controllers) {
                if(controllers[key].method & method) {
                    return controllers[key];
                }
            }
            return false;
       }
       var controller;
       if(_Static[req.url]) {
           controller=checkMethod(_Static[req.url],method);
       } 
       return controller && controller || checkMethod(_RegExp.filter(function(x) {x.pattern.lastIndex=0;return x.pattern.test(req.url) }),method);
};




///////////////////////////////////////////////////////
//
//			Simples
//
///////////////////////////////////////////////////////

http.ServerResponse.prototype.simpleText = function (code, body, extra_headers) {
	this.sendHeader(code, (extra_headers || []).concat([["Content-Type", "text/plain"],["Content-Length", body.length]]));
	this.write(body);
	this.close();
};

http.ServerResponse.prototype.simpleHtml = function (code, body, extra_headers) {
	this.sendHeader(code, (extra_headers || []).concat([["Content-Type", "text/html"],["Content-Length", body.length]]));
	this.write(body);
	this.close();
};

http.ServerResponse.prototype.simpleRedirect = function (location,code) {
	this.sendHeader(code || 302,[["Location", location]]);
	this.close();
};

http.ServerResponse.prototype.simple = function (code,type,body) {
	this.sendHeader(code, [["Content-Type", type],["Content-Length", body.length]]);
	this.write(body);
	this.close();
};

http.ServerResponse.prototype.simpleJson = function (code, json, extra_headers) {
	var body = JSON.stringify(json);
	this.sendHeader(code, (extra_headers || []).concat([["Content-Type", "application/json"],["Content-Length", body.length]]));
	this.write(body);
	this.close();
};

var _sendHeader=http.ServerResponse.prototype.sendHeader;
http.ServerResponse.prototype.sendHeader=function(code,header) {
    var ServerName = 'SeoDyatel/JS';
    if(!header) {
        header=[["Server",ServerName]];
    } else if(header instanceof Array) {
        header=header.concat([["Server",ServerName]]);
    } else { 
        header['Server']=ServerName;
    }
    _sendHeader.call(this,code, header);
}

///////////////////////////////////////////////////////
//
//			Errors
//
///////////////////////////////////////////////////////

http.ServerResponse.prototype._customError={}
http.ServerResponse.prototype.simpleError = function (code, body) {
	if(this._customError[code] ) {
		this._customError[code](body);
	} else {
        if(body) {
		if(this.output.length == 0) this.sendHeader(code, [["Content-Type", "text/html"],["Content-Length", body.length]]);
	        this.write(body);
        } else {
		if(this.output.length == 0) this.sendHeader(code);
        }
		this.close();
	}
}





var Ss=function() {    return this  };
Ss.prototype.Route = new Route();
Ss.prototype.cleanTmpFile = function(list) {
	var file=list.pop();
	fs.stat(file,function(e,stat) {
		if(!e) {
			fs.unlink(file);
		}
		if(list.length>0) {
			setTimeout(ss.cleanTmpFile,500,list);
		}
	});

}

Ss.prototype.serve = function(cfg) {
	ss.cfg =  cfg || {
	    'port':8000,
	    'upload':'/tmp/ss_'
	};
	
	http.createServer(function(req,res) {
		var uri=req.url.replace(/\/\/+/g,"/");
		if(uri != req.url) {
			res.simpleRedirect(uri);
			return;
		}
		
		var call = new callController();
		call.addListener('notfound',function() { 
			res.simpleError(404) 
		});
		call.addListener('internal',function(error) {
			res.simeplError(500,error);
		});
		call.addListener('found',function(controller,match) {
			try {
				controller.callback(req,res,match);
			} catch(e) {
				controller.callback = displayError(e);
				controller.callback(req,res,match);
			}
			
		});

		call.addListener('post',function(controller,match) {

			req.addListener('end',function(err) {
				var del=[]
				if(req.FILES) {
					for(var key in req.FILES) {
						del.push(req.FILES[key].local);
					}
				}
				setTimeout(ss.cleanTmpFile,500,del);
			});

			call.parsePOST(call,req,controller,match);
		});

		call.perform(req);

	}).listen(ss.cfg.port);
}
var ss = new Ss();
exports.listen = ss.serve;
exports.mount = ss.Route.mount;
exports.Controller = Controller;

