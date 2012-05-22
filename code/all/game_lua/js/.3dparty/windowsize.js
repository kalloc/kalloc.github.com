/**
 * Window extensions
 * @namespace Ineqs.Client.Window
 * @copyright 2007
 * @author Alexey Churkin
 */
/**
 *
 */
Ineqs.Client.Window = 
{
	/**
	 * Determines whether the window's DOM tree is ready and can be manipulated by the DOM API
	 * @return {Boolean}
	 */
	isDomReady: function()
	{
		return typeof(document.getElementsByTagName) != typeof(undefined) &&
		(document.getElementsByTagName('body')[0] != null || document.body != null);
	},
	
	setAnchor: function(anchor)
	{
		//we cannot assign an empty hash because some
		//browsers totally reload the page
		var s = "" + window.location;
		s = s.replace(/#.*/, '');
		window.location.replace(s + '#' + anchor.replace(/^#/));
	},
	
	/**
	 * Gets the dimensions of the viewport (excluding scrollbars)
	 * @return {x, y}
	 */
	getViewportSize: function()
	{
		var width = 0;
		var height = 0;
		
		if (window.innerWidth) 
		{
			width = window.innerWidth;
			height = window.innerHeight;
		}
		else 
		{
			if (document.documentElement && document.documentElement.clientWidth) 
			{
				width = document.documentElement.clientWidth;
				height = document.documentElement.clientHeight;
			}
			else 
			{
				if (document.body) 
				{
					if (document.compatMode == "CSS1Compat" && document.body.parentNode.clientWidth) 
					{
						width = document.body.parentNode.clientWidth;
						height = document.body.parentNode.clientHeight;
					}
					else 
					{
						width = document.body.clientWidth;
						height = document.body.clientHeight;
					}
				}
			}
		}
		
		return {
			x: width,
			y: height
		};
	},
	
	getViewportHeight: function()
	{
		return this.getViewportSize().y
	},
	
	getViewportWidth: function()
	{
		return this.getViewportSize().x
	},
	
	/**
	 * Gets the dimensions of the whole window, that could be scrolled
	 * @return {x, y}
	 */
	getScrollableSize: function()
	{
		var yWithScroll = 0;
		var xWithScroll = 0;
		if (window.innerHeight && window.scrollMaxY) 
		{// Firefox
			yWithScroll = window.innerHeight + window.scrollMaxY;
			xWithScroll = window.innerWidth + window.scrollMaxX;
		}
		else 
		{
			if (document.body.scrollHeight > document.body.offsetHeight) 
			{ // all but Explorer Mac
				yWithScroll = document.body.scrollHeight;
				xWithScroll = document.body.scrollWidth;
			}
			else 
			{ // works in Explorer 6 Strict, Mozilla (not FF) and Safari
				yWithScroll = document.body.offsetHeight;
				xWithScroll = document.body.offsetWidth;
			}
		}
		return {
			x: xWithScroll,
			y: yWithScroll
		}
	},
	
	/**
	 * Gets the width of the whole scrollable window
	 * @return {Number}
	 */
	getScrollableWidth: function()
	{
		return this.getScrollableSize().x;
	},
	
	/**
	 * Gets the height of the whole scrollable window
	 * @return {Number}
	 */
	getScrollableHeight: function()
	{
		return this.getScrollableSize().y;
	},
	
	/**
	 * Gets the coordinates of the top left angle of the viewport relative to the whole page
	 * @return {x, y}
	 */
	getViewportCoordinates: function()
	{
		return {
			x: this.pageXOffset || document.documentElement.scrollLeft || document.body.scrollLeft,
			y: this.pageYOffset || document.documentElement.scrollTop || document.body.scrollTop
		}
	},
	
	/**
	 * Returns an integer representing the scrollLeft of the window (the number of pixels the window has scrolled from the left).
	 * @return {Number}
	 */
	getScrollLeft: function()
	{
		return this.getViewportCoordinates().x
	},
	
	/**
	 * Returns an integer representing the scrollTop of the window (the number of pixels the window has scrolled from the top).
	 * @return {Number}
	 */
	getScrollTop: function()
	{
		return this.getViewportCoordinates().y
	}
}
