if((ptr=qsm_get_param("Ccode")) == NULL || cgiFormString ("captcha",REG_CAP,REG_CAP_SIZE+1) || strcmp(REG_CAP,ptr)) {
	    cgiWriteEnd("<register><captcha>error</captcha></register>");	   
}