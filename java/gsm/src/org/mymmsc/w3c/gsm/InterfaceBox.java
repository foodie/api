/**
 * @(#)Communication.java	6.3.9 09/10/02
 *
 * Copyright 2000-2010 MyMMSC Software Foundation (MSF), Inc. All rights reserved.
 * MyMMSC PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */
package org.mymmsc.w3c.gsm;

/**
 * ��Ϣ�����ӿ�
 * 
 * @author WangFeng(wangfeng@yeah.net)
 * @version 6.3.9 09/10/02
 * @since mymmsc-gsm 6.3.9
 */
public interface InterfaceBox {
	/**
	 * �ر�����
	 */
	public void close();

	/**
	 * ���һ����Ϣ
	 * 
	 * @param mail
	 * @return <tt>true</tt>
	 */
	// public boolean add(PduContext mail);

	/**
	 * ȡ��һ���ض�����Ϣ
	 * 
	 * @param index
	 * @return PduContext
	 */
	// public PduContext get(int index);

	/**
	 * ɾ��һ����Ϣ
	 * 
	 * @param mail
	 * @return <tt>true</tt>
	 */
	// public boolean remove(PduContext mail);
}
