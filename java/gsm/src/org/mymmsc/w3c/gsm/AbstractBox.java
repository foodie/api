/**
 * @(#)AbstractBox.java	6.3.9 09/10/02
 *
 * Copyright 2000-2010 MyMMSC Software Foundation (MSF), Inc. All rights reserved.
 * MyMMSC PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */
package org.mymmsc.w3c.gsm;

import java.util.LinkedList;

import org.mymmsc.w3c.gsm.pdu.PduContext;

/**
 * ��Ϣ����������
 * 
 * @author WangFeng(wangfeng@yeah.net)
 * @version 6.3.9 09/10/02
 * @since mymmsc-gsm 6.3.9
 */
public abstract class AbstractBox extends LinkedList<PduContext> implements
		InterfaceBox {
	private static final long serialVersionUID = -5671816341826818905L;

	/** �ż��б� */
	// private List<PduContext> m_mailList = null;

	/**
	 * ���캯��
	 */
	public AbstractBox() {
		// TODO Auto-generated constructor stub
		super();
		// m_mailList = new LinkedList<PduContext>();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.mymmsc.w3c.gsm.InterfaceBox#close()
	 */
	@Override
	public void close() {
		// TODO Auto-generated method stub
		clear();
	}

}
