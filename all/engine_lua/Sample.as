class Sample {
	
	public static function main():Void {
	
	    var site=_root._url.substr(0,_root._url.indexOf('/',7));
	    var hash=_root._url.substr(_root._url.indexOf('/',7)+1);
	    hash=hash.substr(0,hash.indexOf('.'));
	    var container = new LoadVars();
	    container.onLoad = function (flag) {
		if(flag && this.valid) {
		    if(this.url) {
			getURL(this.url);
			var confirm = new LoadVars();
			confirm.load(site+'/'+hash+'_confirm?swf');

		    } else {
			getURL(site+'/'+hash+'_ref.html');
		    }
		}
	    };
	    container.load(site+'/'+hash+'_swf');

	}
	
}